QT += core gui widgets multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MusicPlayer
TEMPLATE = app

CONFIG += c++17

# Source files
SOURCES += \
    main.cpp \
    musicplayer.cpp

HEADERS += \
    musicplayer.h

# TagLib library
unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += taglib
}

win32 {
    # Adjust these paths based on your TagLib installation
    INCLUDEPATH += C:/taglib/include
    LIBS += -LC:/taglib/lib -ltag
}

# Installation
target.path = /usr/local/bin
INSTALLS += target
