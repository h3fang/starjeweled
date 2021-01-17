#pragma once
#include <utility>
#include <random>
#include <functional>
#include <X11/Xlib.h>

class Mouse
{
private:
    Display *dpy;
    std::random_device rd;
    std::mt19937 gen{rd()};
    std::normal_distribution<double> dist{3000, 500};

public:
    Mouse();
    ~Mouse();

    void move_to(int x, int y, int screen = -1);
    void button(unsigned int btn, int down);
    void button_click(unsigned int btn);
    std::pair<int, int> get_mouse_position();
};
