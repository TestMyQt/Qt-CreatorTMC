# https://stackoverflow.com/questions/18646778
TARGET = z            # The name of the output library
TEMPLATE = lib        # Tells Qt that we are compiling a library
                      # the output will be bundled into a .a or .so file
CONFIG += staticlib   # Tells Qt that we want a static library
                      # the output will be a .a file.
                      # Remove this to get a .so file.

include(zlib.pri) # include files

QMAKE_CFLAGS_WARN_ON -= -Wall # Optional - disable warnings when compiling this library
QMAKE_CXXFLAGS_WARN_ON -= -Wall # Optional - disable warnings when compiling this library

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../zlib/release/ -lz
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../zlib/debug/ -lz
