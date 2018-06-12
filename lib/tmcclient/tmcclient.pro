QT       -= gui

TARGET = tmcclient
TEMPLATE = lib
CONFIG += staticlib

QT += network

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    tmcclient.cpp \
    course.cpp \
    exercise.cpp \
    organization.cpp \
    submission.cpp \
    testcase.cpp

HEADERS += \
    tmcclient.h \
    course.h \
    exercise.h \
    organization.h \
    submission.h \
    testcase.h

