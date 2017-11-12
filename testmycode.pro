TEMPLATE = subdirs

SUBDIRS = \
        src \
        3rdparty \
        lib/tmcclient

lib/tmcclient.depends = 3rdparty
src.depends = lib/tmcclient 3rdparty

test {
    message(Configuring test build)
    SUBDIRS += test
    test.depends += 3rdparty
    test.depends += lib/tmcclient
}
