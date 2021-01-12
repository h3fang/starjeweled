#include "mouse.h"
#include <X11/extensions/XTest.h>
#include <stdlib.h>
#include <unistd.h>

Mouse::Mouse() : dpy(XOpenDisplay(NULL)) {}

Mouse::~Mouse() {
    XCloseDisplay(dpy);
}

void Mouse::move_to(int x, int y, int screen) {
    if (screen == -1) {
        screen = DefaultScreen(dpy);
    }
    XWarpPointer(dpy, None, RootWindow(dpy, screen), 0, 0, 0, 0, x, y);
    XFlush(dpy);
    usleep(rand() % 3000 + 1000);
}

void Mouse::button(unsigned int btn, int down) {
    XTestFakeButtonEvent(dpy, btn, down, CurrentTime);
    XFlush(dpy);
}

void Mouse::button_click(unsigned int btn) {
    button(btn, True);
    usleep(rand() % 5000 + 1000);
    button(btn, False);
}

std::pair<int, int> Mouse::get_mouse_position() {
    int ret = False;
    std::pair<int, int> r{-1, -1};
    Window window = 0, root = 0;
    int dummy_int = 0;
    unsigned int dummy_uint = 0;

    for (int i = 0; i < ScreenCount(dpy); i++) {
        Screen *screen = ScreenOfDisplay(dpy, i);
        ret = XQueryPointer(dpy, RootWindowOfScreen(screen), &root, &window, &r.first, &r.second, &dummy_int,
                            &dummy_int, &dummy_uint);
        if (ret == True) {
            break;
        }
    }

    return r;
}
