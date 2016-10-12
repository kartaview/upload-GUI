TEMPLATE = subdirs
SUBDIRS += \
    HTTPRequest \
    QTQMLUtils  \
    UploadComponent \
    KQOAuth \
    zlib

CONFIG += c++11

UploadComponent.depends = HTTPRequest KQOAuth QTQMLUtils zlib
