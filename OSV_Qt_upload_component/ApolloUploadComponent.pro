TEMPLATE = subdirs
SUBDIRS += \
    HTTPRequest \
#    OSVAPI \
#    JOSMAPI \
#    ApolloMapEditor \
    QTQMLUtils  \
    UploadComponent \
    KQOAuth \
    zlib

CONFIG += c++11

 # what subproject depends on others
#ApolloMapEditor.depends = HTTPRequestII QTQMLUtils
#OSVAPI.depends = QTQMLUtils HTTPRequest
#JOSMAPI.depends = HTTPRequest
UploadComponent.depends = HTTPRequest KQOAuth QTQMLUtils zlib

unix {
    SUBDIRS += libusb
    UploadComponent.depends += libusb
}
