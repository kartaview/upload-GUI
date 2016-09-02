#-------------------------------------------------
#
# Project created by QtCreator 2016-04-22T20:59:41
#
#-------------------------------------------------

QT       += qml quick

TARGET = QTQMLUtils
TEMPLATE = lib

CONFIG += c++11 staticlib

DEFINES += OSVAPI_LIBRARY

SOURCES += qtqmlutils.cpp \
    qqmlhelpers.cpp \
    qqmlobjectlistmodel.cpp \
    qqmlvariantlistmodel.cpp \
    qquickpolygon.cpp

HEADERS += qtqmlutils.h \
    qqmlhelpers.h \
    qqmlmodels.h \
    qqmlobjectlistmodel.h \
    qqmlvariantlistmodel_p.h \
    qqmlvariantlistmodel.h \
    qquickpolygon.h \
    qtbitstream.h \
    qtcobs.h \
    qtjsonpath.h
unix,mac {
    target.path = /usr/lib/
    INSTALLS += target
}
