DEFINES += TESTMYCODE_LIBRARY
QT += network widgets

CONFIG += c++11
DEFINES += QUAZIP_STATIC

win32:CONFIG(release, debug|release) {
    LIBS += -L$$OUT_PWD/../lib/tmcclient/release -ltmcclient
    LIBS += -L$$OUT_PWD/../3rdparty/quazip/quazip/release/ -lquazip
    LIBS += -L$$OUT_PWD/../3rdparty/zlib/release/ -lz

    win32-g++ {
        PRE_TARGETDEPS += $$OUT_PWD/../lib/tmcclient/release/libtmcclient.a
        PRE_TARGETDEPS += $$OUT_PWD/../3rdparty/quazip/quazip/release/libquazipd.a
        PRE_TARGETDEPS += $$OUT_PWD/../3rdparty/zlib/release/libz.a
    }

    win32-msvc* {
        PRE_TARGETDEPS += $$OUT_PWD/../lib/tmcclient/release/tmcclient.lib
        PRE_TARGETDEPS += $$OUT_PWD/../3rdparty/quazip/quazip/release/quazip.lib
        PRE_TARGETDEPS += $$OUT_PWD/../3rdparty/zlib/release/z.lib
    }



} else:win32:CONFIG(debug, debug|release) {
    LIBS += -L$$OUT_PWD/../lib/tmcclient/debug/ -ltmcclient
    LIBS += -L$$OUT_PWD/../3rdparty/quazip/quazip/debug/ -lquazipd
    LIBS += -L$$OUT_PWD/../3rdparty/zlib/debug/ -lz

    win32-g++ {
        PRE_TARGETDEPS += $$OUT_PWD/../lib/tmcclient/debug/libtmcclient.a
        PRE_TARGETDEPS += $$OUT_PWD/../3rdparty/quazip/quazip/debug/libquazipd.a
        PRE_TARGETDEPS += $$OUT_PWD/../3rdparty/zlib/debug/libz.a
    }

    win32-msvc* {
        PRE_TARGETDEPS += $$OUT_PWD/../lib/tmcclient/debug/tmcclient.lib
        PRE_TARGETDEPS += $$OUT_PWD/../3rdparty/quazip/quazip/debug/quazipd.lib
        PRE_TARGETDEPS += $$OUT_PWD/../3rdparty/zlib/debug/z.lib
    }

} else:unix:CONFIG(release, debug|release){
    LIBS += -L$$OUT_PWD/../lib/tmcclient/ -ltmcclient
    LIBS += -L$$OUT_PWD/../3rdparty/quazip/quazip/ -lquazip
    LIBS += -L$$OUT_PWD/../3rdparty/zlib/ -lz


    PRE_TARGETDEPS += $$OUT_PWD/../lib/tmcclient/libtmcclient.a
    PRE_TARGETDEPS += $$OUT_PWD/../3rdparty/quazip/quazip/libquazip.a
    PRE_TARGETDEPS += $$OUT_PWD/../3rdparty/zlib/libz.a

} else:unix:CONFIG(debug, debug|release){

    macx: DEBUG_POSTFIX += _debug

    LIBS += -L$$OUT_PWD/../lib/tmcclient/ -ltmcclient
    LIBS += -L$$OUT_PWD/../3rdparty/quazip/quazip/ -lquazip$${DEBUG_POSTFIX}
    LIBS += -L$$OUT_PWD/../3rdparty/zlib/ -lz


    PRE_TARGETDEPS += $$OUT_PWD/../lib/tmcclient/libtmcclient.a
    PRE_TARGETDEPS += $$OUT_PWD/../3rdparty/quazip/quazip/libquazip$${DEBUG_POSTFIX}.a
    PRE_TARGETDEPS += $$OUT_PWD/../3rdparty/zlib/libz.a

}


INCLUDEPATH += $$PWD/../3rdparty/quazip/ \
               $$PWD/../3rdparty/zlib/zlib/ \
               $$PWD/../lib/tmcclient/

DEPENDPATH += $$PWD/../3rdparty/quazip/ \
              $$PWD/../3rdparty/zlib/zlib/ \
              $$PWD/../lib/tmcclient/

# TestMyCode files

HEADERS += testmycode.h \
    testmycode_global.h \
    testmycodeconstants.h \
    tmcoutputpane.h \
    tmctestresult.h \
    tmcresultmodel.h \
    downloadpanel.h \
    settingswidget.h \
    loginwidget.h \
    tmcmanager.h \
    submitwidget.h \
    tmcresultreader.h \
    ziphelper.h \
    exercisewidget.h \
    exercisedelegate.h \
    exercisemodel.h

SOURCES += testmycode.cpp \
    tmcoutputpane.cpp \
    tmctestresult.cpp \
    tmcresultmodel.cpp \
    downloadpanel.cpp \
    settingswidget.cpp \
    loginwidget.cpp \
    tmcmanager.cpp \
    submitwidget.cpp \
    tmcresultreader.cpp \
    ziphelper.cpp \
    exercisewidget.cpp \
    exercisedelegate.cpp \
    exercisemodel.cpp

FORMS += \
    ui/loginscreen.ui \
    ui/settings.ui \
    ui/submit.ui \
    ui/exercisewidget.ui

# Qt Creator linking

## Either set the IDE_SOURCE_TREE when running qmake,
## or set the QTC_SOURCE environment variable, to override the default setting
isEmpty(IDE_SOURCE_TREE) : IDE_SOURCE_TREE = $$(QTC_SOURCE)
isEmpty(IDE_SOURCE_TREE) : IDE_SOURCE_TREE = $$QTC_SOURCE
!exists($$IDE_SOURCE_TREE): \
    error("Set environment variable QTC_SOURCE to QtCreator's sources path (current path is \"$$IDE_SOURCE_TREE\")")

## Either set the IDE_BUILD_TREE when running qmake,
## or set the QTC_BUILD environment variable, to override the default setting
isEmpty(IDE_BUILD_TREE): IDE_BUILD_TREE = $$(QTC_BUILD)
isEmpty(IDE_BUILD_TREE): IDE_BUILD_TREE = $$QTC_BUILD
!exists($$IDE_BUILD_TREE): \
    error("Set environment variable QTC_BUILD to QtCreator's libraries path (current path is \"$$IDE_BUILD_TREE\")")

## If using non-default Qt Creator build, such as Ubuntu PPA build
## we need to set the library base to "lib/x86_64-linux-gnu" instead of "lib"
isEmpty(IDE_LIBRARY_BASENAME): IDE_LIBRARY_BASENAME = $$(QTC_LIBRARY_BASENAME)
isEmpty(IDE_LIBRARY_BASENAME): IDE_LIBRARY_BASENAME = $$QTC_LIBRARY_BASENAME
!isEmpty(IDE_LIBRARY_BASENAME): \
    message("Using library basename: $$IDE_LIBRARY_BASENAME")

## uncomment to build plugin into user config directory
## <localappdata>/plugins/<ideversion>
##    where <localappdata> is e.g.
##    "%LOCALAPPDATA%\QtProject\qtcreator" on Windows Vista and later
##    "$XDG_DATA_HOME/data/QtProject/qtcreator" or "~/.local/share/data/QtProject/qtcreator" on Linux
##    "~/Library/Application Support/QtProject/Qt Creator" on OS X
# USE_USER_DESTDIR = yes

###### If the plugin can be depended upon by other plugins, this code needs to be outsourced to
###### <dirname>_dependencies.pri, where <dirname> is the name of the directory containing the
###### plugin's sources.

QTC_PLUGIN_NAME = TestMyCode
QTC_LIB_DEPENDS += \
    # nothing here at this time

QTC_PLUGIN_DEPENDS += \
    coreplugin \
    projectexplorer \
    autotest

QTC_PLUGIN_RECOMMENDS += \
    # optional plugin dependencies. nothing here at this time

###### End _dependencies.pri contents ######

include($$IDE_SOURCE_TREE/src/qtcreatorplugin.pri)

## Define compilation destination directory in either environment or argument variable
message("Default output path: $$DESTDIR")
isEmpty(BUILD_OUTPUT_PATH) : BUILD_OUTPUT_PATH = $$(BUILD_OUTPUT_PATH)
!isEmpty(BUILD_OUTPUT_PATH) {
    DESTDIR = $$BUILD_OUTPUT_PATH
    message("Plugin output path set to: $$DESTDIR")
}
message("Plugin output path: $$DESTDIR")
