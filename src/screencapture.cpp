#include "screencapture.h"
#include <string.h>
#include "x.hpp"

Window window_from_name_search(Display *display, Window current, char const *target) {
    /* Check if this window has the name we seek */
    XTextProperty text;
    if (XGetWMName(display, current, &text) > 0 && text.nitems > 0) {
        int count = 0;
        char **list = NULL;
        if (Xutf8TextPropertyToTextList(display, &text, &list, &count) == Success && count > 0) {
            const char *r = strstr(list[0], target);
            if (r != NULL) {
                XFree(text.value);
                XFreeStringList(list);
                return current;
            }
        }
        XFreeStringList(list);
    }
    XFree(text.value);

    Window retval = 0, root, parent, *children;
    unsigned children_count;

    /* If it does not: check all subwindows recursively. */
    if (0 != XQueryTree(display, current, &root, &parent, &children, &children_count)) {
        for (unsigned i = 0; i < children_count; ++i) {
            Window win = window_from_name_search(display, children[i], target);

            if (win != 0) {
                retval = win;
                break;
            }
        }

        XFree(children);
    }

    return retval;
}

ScreenCapture::ScreenCapture() :
    display(XOpenDisplay(NULL)),
    root_win(XDefaultRootWindow(display)),
    img(nullptr) {
}

ScreenCapture::~ScreenCapture() {
    XCloseDisplay(display);
}

bool ScreenCapture::capture(const std::string &window_title, const int x, const int y, const int width, const int height) {
    if (img) {
        XDestroyImage(img);
        img = nullptr;
    }
    Window wid = window_from_name_search(display, root_win, window_title.data());
    if (wid == 0) {
        return false;
    }
    img = X11("").getImage(wid, x, y, width, height);
    return img != nullptr;
}

unsigned long ScreenCapture::get_pixel(int x, int y) {
    return XGetPixel(img, x, y);
}

bool ScreenCapture::is_window_visible(const std::string &title, const int min_width, const int min_height) {
    Window wid = window_from_name_search(display, root_win, title.data());
    if (wid == 0) {
        return false;
    }
    XWindowAttributes attr;
    if (XGetWindowAttributes(display, wid, &attr) == False ||
        (min_width > 0 && attr.width < min_width) ||
        (min_height > 0 && attr.height < min_height) ||
        attr.map_state != IsViewable) {
        return false;
    }
    return true;
}
