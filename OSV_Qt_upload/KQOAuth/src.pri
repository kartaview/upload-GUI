QT *= network script

INCLUDEPATH += $$PWD

SOURCES += \
    $$PWD/kqoauthmanager.cpp \
    $$PWD/kqoauthrequest.cpp \
    $$PWD/kqoauthutils.cpp \
    $$PWD/kqoauthauthreplyserver.cpp \
    $$PWD/kqoauthrequest_1.cpp \
    $$PWD/kqoauthrequest_xauth.cpp

HEADERS += \
    $$PWD/kqoauthmanager.h \
    $$PWD/kqoauthrequest.h \
    $$PWD/kqoauthrequest_1.h \
    $$PWD/kqoauthrequest_xauth.h \
    $$PWD/kqoauthglobals.h
