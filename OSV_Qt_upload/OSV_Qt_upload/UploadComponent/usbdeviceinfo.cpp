#include "usbdeviceinfo.h"
#include<qdebug.h>

const int INVALID_DATA = -1;

UsbDeviceInfo::UsbDeviceInfo(libusb_device *device)
    : m_interfaceClass(INVALID_DATA),
      m_serialNr(INVALID_DATA),
      m_err(NO_ERROR)
{
    m_err = initDeviceInfo(device);
}

bool UsbDeviceInfo::operator==(const UsbDeviceInfo& devInfo) const
{
    return m_manufacturer == devInfo.m_manufacturer &&
           m_product == devInfo.m_product &&
           m_interfaceClass == devInfo.m_interfaceClass &&
           m_serialNr == devInfo.m_serialNr;
}

int UsbDeviceInfo::getDeviceInterfaceClass(libusb_device* device)
{
    libusb_config_descriptor *configDescr;
    int res = libusb_get_config_descriptor(device,0,&configDescr);
    if(res != 0)
    {
        return INVALID_DATA;
    }
    return configDescr->interface->altsetting->bInterfaceClass;
}

UsbDeviceInfoError UsbDeviceInfo::initDeviceInfo(libusb_device *device)
{
    struct libusb_device_descriptor desc;
    UsbDeviceInfoError err = NO_ERROR;

    int res = libusb_get_device_descriptor(device, &desc);
    if(res != 0)
    {
        return ERROR_GET_DEVICE_DESCRIPTOR;
    }
    libusb_device_handle *devHandle = NULL;
    res = libusb_open(device, &devHandle);

    if(res != 0)
    {
        return ERROR_OPEN_DEVICE;
    }
    if(devHandle)
    {
        unsigned char buffer[64] = "";
        res = libusb_get_string_descriptor_ascii(devHandle, desc.iManufacturer, buffer, sizeof(buffer));
        if(res >= 0)
        {
            m_manufacturer = QLatin1String((const char*)buffer);
        }
        res = libusb_get_string_descriptor_ascii(devHandle, desc.iProduct, buffer, sizeof(buffer));
        if(res >= 0)
        {
            m_product = QLatin1String((const char*)buffer);
        }
        //skip invalid devices
        if(!m_manufacturer.isEmpty() || !m_product.isEmpty())
        {
            m_serialNr = desc.iSerialNumber;
            m_interfaceClass = getDeviceInterfaceClass(device);
            if(m_interfaceClass == INVALID_DATA || m_serialNr == INVALID_DATA)
            {
                err = ERROR_INVALID_DEVICE_INFO;
            }
        }
        else
        {
            err = ERROR_INVALID_DEVICE_INFO;
        }
        libusb_close(devHandle);
    }

     return err;
}

QString UsbDeviceInfo::toQString()
{
    return QString("Manufacturer: %1 /Product: %2 /InterfaceClass: %3").arg(m_manufacturer).arg(m_product).arg(m_interfaceClass);
}

int UsbDeviceInfo::getInterfaceClassValue()
{
    return m_interfaceClass;
}
bool UsbDeviceInfo::hasErrors()
{
    return m_err != UsbDeviceInfoError::NO_ERROR;
}
