#include "overlay.h"

#include "x.hpp"

int is_window_visible(Display *display, Window wid) {
    XWindowAttributes wattr;
    XGetWindowAttributes(display, wid, &wattr);
    if (wattr.map_state != IsViewable)
        return False;

    return True;
}

Window window_from_name_search(Display *display, Window current, char const *target) {
    Window retval, root, parent, *children;
    unsigned children_count;

    /* Check if this window has the name we seek */
    XTextProperty text;
    if(XGetWMName(display, current, &text) > 0 && text.nitems > 0) {
        int count = 0, result;
        char **list = NULL;
        if (Xutf8TextPropertyToTextList(display, &text, &list, &count) == Success && count > 0) {
            const char* r = strstr(list[0], target);
            if(r != NULL) {
                XFree(text.value);
                XFreeStringList(list);
                return current;
            }
        }
        XFreeStringList(list);
    }
    XFree(text.value);

    retval = 0;

    /* If it does not: check all subwindows recursively. */
    if(0 != XQueryTree(display, current, &root, &parent, &children, &children_count)) {
        unsigned i;
        for(i = 0; i < children_count; ++i) {
            Window win = window_from_name_search(display, children[i], target);

            if(win != 0) {
                retval = win;
                break;
            }
        }

        XFree(children);
    }

    return retval;
}

Overlay::Overlay(QWidget *parent) :
    QWidget(parent, Qt::X11BypassWindowManagerHint | Qt::FramelessWindowHint | Qt::WindowTransparentForInput | Qt::WindowStaysOnTopHint),
    board(N+2*PADDING, vector<int>(N+2*PADDING, -1)),
    display(XOpenDisplay(NULL)),
    root_win(XDefaultRootWindow(display))
{
    setAttribute(Qt::WA_TranslucentBackground);

    setWindowTitle("Terminal");

    resize(window()->screen()->size());

    QTimer *t = new QTimer(this); t->setTimerType(Qt::PreciseTimer);
    connect(t, SIGNAL(timeout()), this, SLOT(update()));
    t->start(100);
}

Overlay::~Overlay() {
    XCloseDisplay(display);
}

Window Overlay::get_window(const char* name) {
    Window r = window_from_name_search(display, root_win, name);
    return r;
}

XImage* Overlay::shootScreen(Window window)
{
    vec2 imageloc;
    auto x = X11("");
    return x.getImage(window ? window : x.root, 0, 0, 1920, 1080, imageloc);
}

void Overlay::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    auto game = get_window((char *)WINDOW_NAME);
    if (!is_window_visible(display, game)) {
        return;
    }

    auto img = shootScreen(game);
    if (img->width != 1920 || img->height != 1080) {
        return;
    }
    get_board(img, p);
    XDestroyImage(img);

#ifndef NDEBUG
    // draw a rectagle around the board (for debugging)
    p.setPen(QPen(Qt::blue, 1.0));
    p.drawRect(QRectF(x0, y0, N * w, N *w));
#endif

    // draw a line for each solution
    p.setPen(QPen(Qt::magenta, 7.0));
    auto solutions = calc_solutions();
    for(const auto& s : solutions) {
        const int x1 = x0 + (s.j1 - PADDING) * w + 0.5 * w, y1 = y0 + (s.i1 - PADDING) * w + 0.5 * w;
        const int x2 = x0 + (s.j2 - PADDING) * w + 0.5 * w, y2 = y0 + (s.i2 - PADDING) * w + 0.5 * w;
        p.drawLine(x1,y1,x2,y2);
    }
}

int Overlay::match_color(const vector<int>& c, vector<vector<int>>& palette, int threshold) {
    for (size_t k = 0; k < palette.size(); k++) {
        const auto p = palette[k];
        if (abs(c[0] - p[0]) < threshold &&
            abs(c[1] - p[1]) < threshold &&
            abs(c[2] - p[2]) < threshold) {
            return k;
        }
    }
    palette.push_back(c);
    return palette.size() - 1;
}

void Overlay::get_board(XImage* img, QPainter& p) {
    Q_UNUSED(p);
    vector<vector<int>> pallete;

    const float cell_w = 0.6 * w, margin = 0.5 * (w - cell_w), n_pixels = cell_w * cell_w;

    for (int m = 0; m < N; ++m ) {
        for (int n =0; n < N; ++n) {
            vector<int> rgb(3);
            const int x = x0 + n * w + margin, y = y0 + m * w + margin;
            for (int i = 0; i < cell_w; ++i) {
                for (int j = 0; j < cell_w; ++j) {
                    const QColor c(XGetPixel(img, x + i, y + j));
                    rgb[0] += c.red();
                    rgb[1] += c.green();
                    rgb[2] += c.blue();
                }
            }
            rgb[0] = rgb[0] / n_pixels;
            rgb[1] = rgb[1] / n_pixels;
            rgb[2] = rgb[2] / n_pixels;
            board[m+PADDING][n+PADDING] = match_color(rgb, pallete, 15);
#ifndef NDEBUG
            p.setPen(Qt::NoPen);
            p.setBrush(QBrush(QColor(rgb[0], rgb[1], rgb[2])));
            p.drawRect(QRect(x-margin, y-margin, margin, margin));
            p.setPen(Qt::red);
            p.setFont(QFont("Monospace", 8));
            p.drawText(x, y, QString::number(board[m+PADDING][n+PADDING]));
#endif
        }
    }
}

Solution* Overlay::get_best_move() {
    auto solutions = calc_solutions();
    if (solutions.empty()) {
        return nullptr;
    }
    sort(solutions.begin(), solutions.end(), [](const auto& lhs, const auto& rhs){
        return lhs.i1 > rhs.i1 || lhs.i2 > rhs.i2;
    });

    return &solutions[0];
}

vector<Solution> Overlay::calc_solutions() {
    vector<Solution> s;
    for (int m = N-1+PADDING; m >= PADDING; --m ) {
        for (int n = PADDING; n < N+PADDING; ++n) {
            const int c = board[m][n], cr = board[m][n+1], ct = board[m-1][n];
            // swap with the right cell
            if (cr != -1) {
                if ((c == board[m][n+2] && c == board[m][n+3])
                 || (c == board[m-1][n+1] && c == board[m-2][n+1])
                 || (c == board[m+1][n+1] && c == board[m+2][n+1])
                 || (c == board[m-1][n+1] && c == board[m+1][n+1])
                 || (cr == board[m][n-1] && cr == board[m][n-2])
                 || (cr == board[m-1][n] && cr == board[m-2][n])
                 || (cr == board[m+1][n] && cr == board[m+2][n])
                 || (cr == board[m-1][n] && cr == board[m+1][n])
                ) {
                    s.push_back({m,n,m,n+1});
                }
            }
            // swap with the top cell
            if (ct != -1) {
                if ((c == board[m-2][n] && c == board[m-3][n])
                 || (c == board[m-1][n-1] && c == board[m-1][n-2])
                 || (c == board[m-1][n+1] && c == board[m-1][n+2])
                 || (c == board[m-1][n-1] && c == board[m-1][n+1])
                 || (ct == board[m+1][n] && ct == board[m+2][n])
                 || (ct == board[m][n-1] && ct == board[m][n-2])
                 || (ct == board[m][n+1] && ct == board[m][n+2])
                 || (ct == board[m][n-1] && ct == board[m][n+1])
                ) {
                    s.push_back({m,n,m-1,n});
                }
            }
        }
    }

    return s;
}
