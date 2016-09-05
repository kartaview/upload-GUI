TARGET   = usblib
TEMPLATE = lib
CONFIG  += staticlib

INCLUDEPATH += .
INCLUDEPATH +=/usr/include/
INCLUDEPATH +=/usr/local/

HEADERS += \
    libusb.h \
    os/darwin_usb.h \
    os/threads_posix.h

SOURCES += \
    core.c \
    descriptor.c \
    io.c \
    sync.c \
    os/darwin_usb.c \
    os/threads_posix.c

LIBS += -framework CoreFoundation -framework Cocoa -framework IOKit

###############################################################################
# Generate library
TN_BUILD_PATH   =  ../../../build
TN_LIBRARY_NAME = usblib
