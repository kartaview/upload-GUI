TEMPLATE = app

QT += qml quick concurrent core network xml

CONFIG += c++11 kqoauth

SOURCES += main.cpp \
    logincontroller.cpp \
    osmlogin.cpp \
    photo.cpp \
    jsonserializable.cpp \
    GZIP.cpp \
    exif.cpp \
    UploadManager.cpp \
    persistentsequence.cpp \
    persistentcontroller.cpp \
    video.cpp \
    metadata.cpp \
    uploadcontroller.cpp \
    elapsedtimecounter.cpp

RESOURCES += qml.qrc

# Default rules for deployment.
include(deployment.pri)

# what subproject depends on others

HEADERS += \
    logincontroller.h \
    osmlogin.h \
    uploadcomponentconstants.h \
    photo.h \
    jsonserializable.h \
    GZIP.h \
    exif.h \
    UploadManager.h \
    persistentsequence.h \
    persistentcontroller.h \
    video.h \
    metadata.h \
    uploadcontroller.h \
    elapsedtimecounter.h



unix {
    SOURCES += usbdevices.cpp \
               usbdeviceinfo.cpp \
               osvdevicescanner.cpp

    HEADERS += usbdevices.h \
               usbdeviceinfo.h \
               osvdevicescanner.h
}

#http libs
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../HTTPRequest/release/ -lHTTPRequest
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../HTTPRequest/debug/ -lHTTPRequest
else:unix: LIBS += -L$$OUT_PWD/../HTTPRequest/ -lHTTPRequest

INCLUDEPATH += $$PWD/../HTTPRequest
DEPENDPATH += $$PWD/../HTTPRequest

#QTQMLUtils libs
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QTQMLUtils/release/ -lQTQMLUtils
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QTQMLUtils/debug/ -lQTQMLUtils
else:unix: LIBS += -L$$OUT_PWD/../QTQMLUtils/ -lQTQMLUtils

INCLUDEPATH += $$PWD/../QTQMLUtils
DEPENDPATH += $$PWD/../QTQMLUtils

#KQOAUTH libs
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../KQOAuth/release/ -lKQOAuth
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../KQOAuth/debug/ -lKQOAuth
else:unix: LIBS += -L$$OUT_PWD/../KQOAuth/ -lKQOAuth

INCLUDEPATH += $$PWD/../KQOAuth
DEPENDPATH += $$PWD/../KQOAuth

#zlib
win32 {
    LIBS+= $$OUT_PWD/../zlib/release/zlib.lib
}
else:unix {
    LIBS += -lz
    LIBS+= $$OUT_PWD/../zlib/libzlib.a
}

INCLUDEPATH += $$PWD/../zlib
DEPENDPATH += $$PWD/../zlib

#usblib
win32 {
   # LIBS+= $$OUT_PWD/../libusb/release/libusblib.lib
}
else:unix {
    LIBS+= $$OUT_PWD/../libusb/libusblib.a
    LIBS += -framework CoreFoundation -framework Cocoa -framework IOKit
    INCLUDEPATH += $$PWD/../libusb/include/libusb-1.0
}

#framework
FRAMEWORKS.files += $$OUT_PWD/../HTTPRequest/libHTTPRequest.1.dylib
FRAMEWORKS.files += $$OUT_PWD/../KQOAuth/libKQOAuth.1.dylib

FRAMEWORKS.path = Contents/Frameworks
QMAKE_BUNDLE_DATA += FRAMEWORKS
