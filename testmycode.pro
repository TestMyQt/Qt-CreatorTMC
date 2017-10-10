DEFINES += TESTMYCODE_LIBRARY

QT += network

# TestMyCode files

test {
    message(Configuring test build)
    QT += testlib network

    SOURCES += \
        test/testmycode_tests.cpp
} else {

SOURCES += src/testmycode.cpp

HEADERS += src/testmycode.h \
        src/testmycode_global.h \
        src/testmycodeconstants.h

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
    coreplugin

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
}

FORMS += \
    loginscreen.ui

STATECHARTS +=

HEADERS += \
    src/authentication.h

SOURCES += \
    src/authentication.cpp
