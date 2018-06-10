TEMPLATE = subdirs

SUBDIRS = \
        src \
        3rdparty \
        tmcclient

tmcclient.subdir = lib/tmcclient
src.depends = tmcclient 3rdparty

test {
    message(Configuring test build)
    SUBDIRS += test

    test.subdirs = lib/tmcclient/test
    test.depends += 3rdparty
    test.depends += tmcclient
}
