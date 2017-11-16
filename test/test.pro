TEMPLATE = app
QT -= gui
QT += network
QT += testlib
CONFIG += console
CONFIG -= app_bundle
DEPENDPATH += .
INCLUDEPATH += .

CONFIG += c++11

INCLUDEPATH += $$PWD/../lib/tmcclient/
DEPENDPATH += $$PWD/../lib/tmcclient/

LIBS += -L$$OUT_PWD/../lib/tmcclient/ -ltmcclient
LIBS += -L$$OUT_PWD/../3rdparty/quazip/quazip/ -lquazip
LIBS += -L$$OUT_PWD/../3rdparty/zlib/ -lz

PRE_TARGETDEPS += $$OUT_PWD/../lib/tmcclient/libtmcclient.a
PRE_TARGETDEPS += $$OUT_PWD/../3rdparty/quazip/quazip/libquazip.a
PRE_TARGETDEPS += $$OUT_PWD/../3rdparty/zlib/libz.a

SOURCES += testmycode_tests.cpp
