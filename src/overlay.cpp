#include "overlay.h"

#include <unistd.h>

#include <QPainter>
#include <QTimer>
#include <QScreen>

#include "mouse.h"

Overlay::Overlay(bool automate, int interval, QWidget *parent) :
    QWidget(parent, Qt::BypassWindowManagerHint | Qt::FramelessWindowHint | Qt::WindowTransparentForInput | Qt::WindowStaysOnTopHint),
    board(N+2*PADDING, vector<int>(N+2*PADDING, -1))
{
    setAttribute(Qt::WA_TranslucentBackground);

    setWindowTitle("Terminal");

    resize(window()->screen()->size());

    QTimer *t = new QTimer(this); t->setTimerType(Qt::PreciseTimer);
    connect(t, SIGNAL(timeout()), this, SLOT(update()));
    t->start(100);

    if (automate) {
        t = new QTimer(this); t->setTimerType(Qt::PreciseTimer);
        connect(t, SIGNAL(timeout()), this, SLOT(make_move()));
        t->start(interval);
    }
}

Overlay::~Overlay() {
}

void Overlay::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    if (!screencapture.is_window_visible((char *)WINDOW_NAME, 1920, 1080)) {
        return;
    }

    if (!screencapture.capture((char *)WINDOW_NAME, x0, y0, N * w, N * w)) {
        return;
    }

    get_board(p);

#ifndef NDEBUG
    // draw a rectagle around the board (for debugging)
    p.setPen(QPen(Qt::blue, 1.0));
    p.setBrush(Qt::NoBrush);
    p.drawRect(QRectF(x0, y0, N * w, N *w));
#endif

    // draw a line for each solution
    p.setPen(QPen(Qt::magenta, 7.0));
    auto solutions = calc_solutions();
    for(const auto& s : solutions) {
        const int x1 = x0 + (s.j1) * w + 0.5 * w, y1 = y0 + (s.i1) * w + 0.5 * w;
        const int x2 = x0 + (s.j2) * w + 0.5 * w, y2 = y0 + (s.i2) * w + 0.5 * w;
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

void Overlay::get_board(QPainter& p) {
    Q_UNUSED(p);
    vector<vector<int>> pallete;

    const float cell_w = 0.6 * w, margin = 0.5 * (w - cell_w), n_pixels = cell_w * cell_w;

    for (int m = 0; m < N; ++m ) {
        for (int n =0; n < N; ++n) {
            vector<int> rgb(3);
            const int x = n * w + margin, y = m * w + margin;
            for (int i = 0; i < cell_w; ++i) {
                for (int j = 0; j < cell_w; ++j) {
                    const QColor c(screencapture.get_pixel(x + i, y + j));
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
            p.drawRect(QRect(x0+x-margin, y0+y-margin, margin, margin));
            p.setPen(Qt::red);
            p.setFont(QFont("Monospace", 8));
            p.drawText(x0+x, y0+y, QString::number(board[m+PADDING][n+PADDING]));
#endif
        }
    }
}

Solution Overlay::get_best_move() {
    auto solutions = calc_solutions();
    if (solutions.empty()) {
        return {-1, -1, -1, -1};
    }
    sort(solutions.begin(), solutions.end(), [](const auto& lhs, const auto& rhs){
        return lhs.i1 > rhs.i1 || lhs.i2 > rhs.i2;
    });

    return solutions[0];
}

void Overlay::make_move() {
    const auto game = window_from_name_search(display, root_win, (char *)WINDOW_NAME);
    if (!is_game_visible(game)) {
        return;
    }
    auto p = get_mouse_position(display);
    if (p.first < x0 || p.first > x0 + N * w || p.second < y0 || p.second > y0 + N * w) {
        return;
    }
    auto s = get_best_move();
    if (s.i1 == -1) {
        return;
    }
    const auto left_btn = 1; //XKeysymToKeycode(display, XK_Pointer_Button1);
    const auto screen = DefaultScreen(display);
    move_mouse(display, (s.j1 + 0.5) * w + x0, (s.i1 + 0.5) * w + y0, screen);
    mouse_click(display, left_btn);
    usleep(rand() % 5000 + 1000);
    move_mouse(display, (s.j2 + 0.5) * w + x0, (s.i2 + 0.5) * w + y0, screen);
    mouse_click(display, left_btn);
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
                    s.push_back({m - PADDING, n - PADDING, m - PADDING, n + 1 - PADDING});
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
                    s.push_back({m - PADDING, n - PADDING, m - 1 - PADDING, n - PADDING});
                }
            }
        }
    }

    return s;
}
