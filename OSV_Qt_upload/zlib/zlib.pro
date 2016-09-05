TARGET   = zlib
TEMPLATE = lib
CONFIG  += staticlib

#include(../../delta/common.pri)

QMAKE_CFLAGS -= -Wall
QMAKE_CFLAGS += -w

INCLUDEPATH += .
INCLUDEPATH +=/usr/include/

DEFINES += Z_PREFIX

HEADERS += \
    zutil.h \
    zlib.h \
    zconf.h \
    trees.h \
    inftrees.h \
    inflate.h \
    inffixed.h \
    inffast.h \
    gzguts.h \
    deflate.h \
    crc32.h

SOURCES += \
    zutil.c \
    uncompr.c \
    trees.c \
    inftrees.c \
    inflate.c \
    inffast.c \
    infback.c \
    gzwrite.c \
    gzread.c \
    gzlib.c \
    gzclose.c \
    deflate.c \
    crc32.c \
    compress.c \
    adler32.c

###############################################################################
# Generate library
TN_BUILD_PATH   =  ../../../build
TN_LIBRARY_NAME = zlib
#include(../../../qmake_framework/target_output_paths.pri)

