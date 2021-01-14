#include "overlay.h"

#include <unistd.h>

#include <QPainter>
#include <QTimer>
#include <QScreen>

#include "mouse.h"

Overlay::Overlay(bool automate, int interval, QWidget *parent) :
    QWidget(parent, Qt::BypassWindowManagerHint | Qt::FramelessWindowHint | Qt::WindowTransparentForInput | Qt::WindowStaysOnTopHint)
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

void Overlay::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    if (!screencapture.is_window_visible((char *)WINDOW_NAME, 1920, 1080)) {
        return;
    }

    if (!screencapture.capture((char *)WINDOW_NAME, X0, Y0, solver.N * W, solver.N * W)) {
        return;
    }

    get_board(p);
    solver.calc_solutions();

#ifndef NDEBUG
    // draw a rectagle around the board (for debugging)
    p.setPen(QPen(Qt::blue, 1.0));
    p.setBrush(Qt::NoBrush);
    p.drawRect(QRectF(X0, Y0, solver.N * W, solver.N *W));
#endif

    // draw a line for each solution
    p.setPen(QPen(Qt::magenta, 7.0));
    for(const auto& [s, v] : solver.solutions) {
        const int x1 = X0 + (s.j1) * W + 0.5 * W, y1 = Y0 + (s.i1) * W + 0.5 * W;
        const int x2 = X0 + (s.j2) * W + 0.5 * W, y2 = Y0 + (s.i2) * W + 0.5 * W;
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

    const float cell_w = 0.6 * W, margin = 0.5 * (W - cell_w), n_pixels = cell_w * cell_w;

    for (int m = 0; m < solver.N; ++m ) {
        for (int n =0; n < solver.N; ++n) {
            vector<int> rgb(3);
            const int x = n * W + margin, y = m * W + margin;
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
            solver.board[m+solver.PADDING][n+solver.PADDING] = match_color(rgb, pallete, 15);
#ifndef NDEBUG
            p.setPen(Qt::NoPen);
            p.setBrush(QBrush(QColor(rgb[0], rgb[1], rgb[2])));
            p.drawRect(QRect(X0+x-margin, Y0+y-margin, margin, margin));
            p.setPen(Qt::red);
            p.setFont(QFont("Monospace", 8));
            p.drawText(X0+x, Y0+y, QString::number(solver.board[m+solver.PADDING][n+solver.PADDING]));
#endif
        }
    }
}

void Overlay::make_move() {
    const auto s = solver.get_best_solution();
    if (s.i1 < 0) {
        return;
    }
    if (!screencapture.is_window_visible((char *)WINDOW_NAME)) {
        return;
    }
    auto p = mouse.get_mouse_position();
    if (p.first < X0 || p.first > X0 + solver.N * W || p.second < Y0 || p.second > Y0 + solver.N * W) {
        return;
    }
    const int left_btn = 1;
    mouse.move_to((s.j1 + 0.5) * W + X0, (s.i1 + 0.5) * W + Y0);
    mouse.button_click(left_btn);
    usleep(rand() % 5000 + 1000);
    mouse.move_to((s.j2 + 0.5) * W + X0, (s.i2 + 0.5) * W + Y0);
    mouse.button_click(left_btn);
}
