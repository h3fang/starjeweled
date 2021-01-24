#include <QApplication>
#include <QCommandLineParser>

#include "overlay.h"

int main(int argc, char *argv[]) {
    if (qEnvironmentVariableIsSet("QT_QPA_PLATFORM") && qEnvironmentVariable("QT_QPA_PLATFORM") != "xcb") {
        printf("setting QT_QPA_PLATFORM to xcb\n");
        qputenv("QT_QPA_PLATFORM", "xcb");
    }
    QApplication app(argc, argv);
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addOptions({
        {{"a", "automation"}, "enable automation"},
        {{"i", "interval"}, "time interval between automated actions", "milliseconds", "1000"},
        {{"t", "threshold"}, "solution age threshold", "milliseconds", "1000"},
    });
    parser.process(app);

    Overlay overlay(parser.isSet("a"), parser.value("i").toUInt(), parser.value("t").toUInt());
    overlay.showMaximized();

    return app.exec();
}
