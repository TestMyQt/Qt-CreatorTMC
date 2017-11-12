QT       -= gui

TARGET = tmcclient
TEMPLATE = lib
CONFIG += staticlib

QT += network

CONFIG += c++11

INCLUDEPATH += $$PWD/../../3rdparty/quazip/

DEPENDPATH += $$PWD/../../3rdparty/quazip/

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        tmcclient.cpp \
        course.cpp \
        exercise.cpp

HEADERS += \
        tmcclient.h \
        course.h \
        exercise.h

