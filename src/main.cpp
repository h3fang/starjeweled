#include <QApplication>
#include <QScreen>

#include "overlay.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Overlay overlay;
    overlay.showMaximized();

    return app.exec();
}
