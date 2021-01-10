#pragma once
#include <X11/Xlib.h>

void move_mouse(Display *dpy, int x, int y, int screen);
void mouse_button(Display *dpy, int button, bool down);
void mouse_click(Display *dpy, int button);
