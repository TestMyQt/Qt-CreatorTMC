TEMPLATE = app
QT -= gui
QT += network
QT += testlib
CONFIG += console
CONFIG -= app_bundle
DEPENDPATH += .
INCLUDEPATH += .

SOURCES += testmycode_tests.cpp

INCLUDEPATH += $$PWD/../src
DEPENDPATH += $$PWD/../src
