#ifndef USBDEVICES_H
#define USBDEVICES_H

#include <QObject>
#include "qqmlhelpers.h"
#include<libusb.h>
#include<QList>
#include"usbdeviceinfo.h"

class UsbDevices: public QObject
{
    Q_OBJECT
    QML_READONLY_PROPERTY (QString, lastDetectedDeviceInfo)

    enum UsbDeviceFilterType : unsigned char
    {
        INTERFACE_CLASS_NOT_SUPPORTED = 0,
        INTERFACE_CLASS_PTP = 1,
        INTERFACE_CLASS_MASS_STORAGE = 1 << 1
    };
public:
    UsbDevices(QObject *parent=NULL);
    void addFilter(libusb_class_code filterType);
    ~UsbDevices();

 public slots:
    void scanUsbDevices();

private:
    UsbDevices::UsbDeviceFilterType libClassCodeToUsbDeviceFilterType(libusb_class_code val);
    bool updateListOfFoundDevices(libusb_device **devices, int deviceCount);
    bool filterDeviceInfo(UsbDeviceInfo* devInfo);
    int addElementsToValidDevices(QList<UsbDeviceInfo> *refList);
    int removeElementsFromValidDevices(QList<UsbDeviceInfo> *refList);

    QList<UsbDeviceInfo> m_foundDevices;
    unsigned char m_filters;
};

#endif // USBDEVICES_H
