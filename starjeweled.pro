QT += core widgets
CONFIG += c++17 warn_on

TARGET = terminal

release {
    DEFINES += NDEBUG
}

HEADERS           = src/mouse.h \
                    src/overlay.h \
                    src/screencapture.h \
                    src/solver.h \
                    src/x.hpp

SOURCES           = src/main.cpp \
                    src/mouse.cpp \
                    src/overlay.cpp \
                    src/screencapture.cpp \
                    src/solver.cpp \
                    src/x.cpp \

LIBS += -lX11 -lXext -lXrandr -lXrender -lXcomposite -lXfixes -lXtst
