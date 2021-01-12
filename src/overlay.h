#pragma once

#include <vector>
#include <QWidget>

#include "screencapture.h"
#include "solver.h"
#include "mouse.h"

class Overlay : public QWidget
{
    Q_OBJECT

    const uchar WINDOW_NAME[13] = {0xe6, 0x98, 0x9f, 0xe9, 0x99, 0x85, 0xe4, 0xba, 0x89, 0xe9, 0x9c, 0xb8, 0};
    // board geometry for resolution 1920x1080
    const int W = 72, X0 = 1305, Y0 = 101;

    ScreenCapture screencapture;
    Solver solver;
    Mouse mouse;

public:
    explicit Overlay(bool automate = true, int interval = 1000, QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *) override;

protected slots:
    void make_move();

private:
    int match_color(const vector<int>& c, vector<vector<int>>& palette, int threshold = 10);
    void get_board(QPainter& p);
};
