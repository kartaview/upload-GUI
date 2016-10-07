unix:!android {
    isEmpty(target.path) {
        qnx {
            target.path = /tmp/$${TARGET}/bin
        } else {
            target.path = /opt/$${TARGET}/bin
        }
        export(target.path)
    }
    INSTALLS += target
}

export(INSTALLS)

top_srcdir=$$PWD
top_builddir=$$shadowed($$PWD)

macx: {
    #set RPATH (place to look for .dylib & framework by default)
    QMAKE_RPATHDIR += @executable_path/../Frameworks

    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7

    deploy: QMAKE_POST_LINK = $(QTDIR)/bin/macdeployqt $${TARGET}.app -qmldir=$${top_srcdir}
}

win32: {
    deploy: QMAKE_POST_LINK = $(QTDIR)/bin/windeployqt release/$${TARGET}.exe -qmldir=$${top_srcdir}
}
