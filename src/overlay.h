#pragma once

#include <vector>

#include <QWidget>

#include "screencapture.h"
#include "mouse.h"

using std::vector;

struct Solution {
    int i1, j1;
    int i2, j2;
    bool operator==(const Solution& rhs) const {
        return i1 == rhs.i1 && j1 == rhs.j1 && i2 == rhs.i2 && j2 == rhs.j2;
    }
    bool operator<(const Solution& rhs) const {
        return i1 < rhs.i1 || i2 < rhs.i2 || j1 < rhs.j1 || j2 < rhs.j2;
    }
};

class Overlay : public QWidget
{
    Q_OBJECT

    const uchar WINDOW_NAME[13] = {0xe6, 0x98, 0x9f, 0xe9, 0x99, 0x85, 0xe4, 0xba, 0x89, 0xe9, 0x9c, 0xb8, 0};
    const int N = 8, PADDING = 3;
    // board geometry for resolution 1920x1080
    const int w = 72, x0 = 1305, y0 = 101;

    vector<vector<int>> board;
    ScreenCapture screencapture;
    Mouse mouse;

public:
    explicit Overlay(bool automate = true, int interval = 1000, QWidget *parent = 0);
    ~Overlay();

protected:
    void paintEvent(QPaintEvent *) override;

protected slots:
    void make_move();

private:
    int match_color(const vector<int>& c, vector<vector<int>>& palette, int threshold = 10);
    void get_board(QPainter& p);
    bool is_solution(const int m, const int n, const int dx, const int dy);
    vector<Solution> calc_solutions();
    Solution get_best_move();
};
