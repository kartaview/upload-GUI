#-------------------------------------------------
#
# Project created by QtCreator 2016-04-13T20:17:42
#
#-------------------------------------------------

QT       += network core

QT       -= gui

CONFIG += c++11

TARGET = HTTPRequest
TEMPLATE = lib

DEFINES += HTTPREQUEST_LIBRARY

SOURCES += httprequest.cpp

HEADERS += httprequest.h\
        httprequest_global.h

unix,mac {
    target.path = /usr/lib
    INSTALLS += target
}
