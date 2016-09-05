#-------------------------------------------------
#
# Project created by QtCreator 2016-04-13T20:21:20
#
#-------------------------------------------------

QT       += gui network core positioning

TARGET = OSVAPI
TEMPLATE = lib

CONFIG += c++11 create_prl link_prl

DEFINES += OSVAPI_LIBRARY

SOURCES += skosvapimanager.cpp \
    metadata.cpp \
    photo.cpp \
    sequence.cpp \
    skosvapirequestbuilder.cpp \
    user.cpp \
    jsonserializable.cpp \
    metadatainfo.cpp \
    utils.cpp \
    photodetection.cpp \
    metadataqtwrapper.cpp

HEADERS += skosvapimanager.h\
        osvapi_global.h \
    metadata.h \
    photo.h \
    sequence.h \
    skosvapiconstants.h \
    skosvapirequestbuilder.h \
    user.h \
    jsonserializable.h \
    metadatainfo.h \
    utils.h \
    photodetection.h \
    metadataqtwrapper.h

unix,mac {
    target.path = /usr/lib
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../HTTPRequest/release/ -lHTTPRequest
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../HTTPRequest/debug/ -lHTTPRequest
else:unix: LIBS += -L$$OUT_PWD/../HTTPRequest/ -lHTTPRequest

INCLUDEPATH += $$PWD/../HTTPRequest
DEPENDPATH += $$PWD/../HTTPRequest

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QTQMLUtils/release/ -lQTQMLUtils
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QTQMLUtils/debug/ -lQTQMLUtils
else:unix: LIBS += -L$$OUT_PWD/../QTQMLUtils/ -lQTQMLUtils

INCLUDEPATH += $$PWD/../QTQMLUtils
DEPENDPATH += $$PWD/../QTQMLUtils
