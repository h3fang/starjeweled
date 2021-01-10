#include "mouse.h"
#include <unistd.h>
#include <math.h>
#include <X11/extensions/XTest.h>

void move_mouse(Display *dpy, int x, int y, int screen)  {
    XWarpPointer(dpy, None, RootWindow(dpy, screen), 0, 0, 0, 0, x, y);
    XFlush(dpy);
    usleep(rand() % 3000 + 1000);
}

void mouse_button(Display *dpy, int button, bool down) {
    XTestFakeButtonEvent(dpy, button, down, CurrentTime);
    XFlush(dpy);
}

void mouse_click(Display *dpy, int button) {
    mouse_button(dpy, button, True);
    usleep(rand() % 5000 + 1000);
    mouse_button(dpy, button, False);
}
