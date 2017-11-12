TEMPLATE = subdirs

SUBDIRS += \
        src \
        3rdparty

src.depends = 3rdparty

test {
    message(Configuring test build)
    SUBDIRS += test
    test.depends = src
}
