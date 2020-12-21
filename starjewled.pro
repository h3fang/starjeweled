QT += widgets
CONFIG += c++17

TARGET = openoffice

release {
    DEFINES += NDEBUG
}

HEADERS           = src/overlay.h \
                    src/x.hpp

SOURCES           = src/main.cpp \
                    src/overlay.cpp \
                    src/x.cpp

LIBS += -lX11 -lXext -lXrandr -lXrender -lXcomposite -lXfixes
