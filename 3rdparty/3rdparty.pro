TEMPLATE = subdirs

SUBDIRS += \
        quazip \
        zlib

quazip.depends = zlib
