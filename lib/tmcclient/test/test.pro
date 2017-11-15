TEMPLATE = app
QT -= gui
QT += network
QT += testlib
QT += core
CONFIG += c++11 console testcase
CONFIG -= app_bundle
DEPENDPATH += .
INCLUDEPATH += .

INCLUDEPATH += $$PWD/../../tmcclient/
DEPENDPATH += $$PWD/../../tmcclient/

LIBS += -L$$OUT_PWD/../../tmcclient/ -ltmcclient
LIBS += -L$$OUT_PWD/../../../3rdparty/quazip/quazip/ -lquazip
LIBS += -L$$OUT_PWD/../../../3rdparty/zlib/ -lz

PRE_TARGETDEPS += $$OUT_PWD/../libtmcclient.a
PRE_TARGETDEPS += $$OUT_PWD/../../../3rdparty/quazip/quazip/libquazip.a
PRE_TARGETDEPS += $$OUT_PWD/../../../3rdparty/zlib/libz.a

HEADERS += \
        testnetworkaccessmanager.h \
        testnetworkreply.h

SOURCES += \
        tmcclient_tests.cpp \
        testnetworkaccessmanager.cpp \
        testnetworkreply.cpp
