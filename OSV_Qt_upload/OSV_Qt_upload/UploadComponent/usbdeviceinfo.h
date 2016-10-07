#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include <QString>
#include <libusb.h>

enum UsbDeviceInfoError
{
    NO_ERROR,
    ERROR_OPEN_DEVICE,
    ERROR_GET_DEVICE_DESCRIPTOR,
    ERROR_GET_CONFIG_DESCRIPTOR,
    ERROR_INVALID_DEVICE_INFO
};


class UsbDeviceInfo
{
public:
    UsbDeviceInfo(libusb_device* device);
    bool operator==(const UsbDeviceInfo& devInfo) const;
    int getDeviceInterfaceClass(libusb_device* device);
    QString toQString();
    int getInterfaceClassValue();
    bool hasErrors();

private:
    UsbDeviceInfoError initDeviceInfo(libusb_device* device);
    QString m_manufacturer;
    QString m_product;
    int m_interfaceClass;
    int m_serialNr;
    UsbDeviceInfoError m_err;
};

#endif // DEVICEINFO_H
