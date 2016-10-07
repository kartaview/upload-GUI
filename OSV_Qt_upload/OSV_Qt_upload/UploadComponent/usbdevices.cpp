#include "usbdevices.h"
#include<qdebug.h>

UsbDevices::UsbDevices(QObject *parent)
    : QObject(parent),
      m_filters(0)
{
    libusb_init(NULL);
}

void UsbDevices::scanUsbDevices()
{
    libusb_device **devices = nullptr;
    const int devCount = libusb_get_device_list(NULL, &devices);

    if(devCount == 0)
    {
        qDebug() << "No USB devices found";
    }
    else if(devCount != m_foundDevices.size())
    {
        updateListOfFoundDevices(devices, devCount);
    }
    libusb_free_device_list(devices, 1); // 1 - decrement reference count of each device in the list

}

UsbDevices::UsbDeviceFilterType UsbDevices::libClassCodeToUsbDeviceFilterType(libusb_class_code val)
{
    switch (val)
    {
        case libusb_class_code::LIBUSB_CLASS_PTP:
            return INTERFACE_CLASS_PTP;
        case libusb_class_code::LIBUSB_CLASS_MASS_STORAGE:
            return INTERFACE_CLASS_MASS_STORAGE;
        default:
            return INTERFACE_CLASS_NOT_SUPPORTED;
    }
}

void UsbDevices::addFilter(libusb_class_code filterType)
{
    UsbDeviceFilterType usbDevFilterType = libClassCodeToUsbDeviceFilterType(filterType);
    if(usbDevFilterType == INTERFACE_CLASS_NOT_SUPPORTED)
    {
        qFatal((QString("Unsuported libusb_class_code: %1").arg(usbDevFilterType)).toStdString().c_str());
    }
    else
    {
        m_filters |= usbDevFilterType;
    }
}
UsbDevices::~UsbDevices()
{
    libusb_exit(NULL);
}

bool UsbDevices::filterDeviceInfo(UsbDeviceInfo* devInfo)
{
    if (!m_filters)
    {
        return true;
    }

    UsbDeviceFilterType devInterfaceClassType = libClassCodeToUsbDeviceFilterType((libusb_class_code)devInfo->getInterfaceClassValue());
    return m_filters & devInterfaceClassType;
}

bool UsbDevices::updateListOfFoundDevices(libusb_device **devices, int deviceCount)
{
    bool devListChanged = false;
    QList<UsbDeviceInfo> currentValidDevices;
    for(int index=0; index < deviceCount; ++index)
    {
        UsbDeviceInfo devInfo(devices[index]);
        if(!devInfo.hasErrors() && filterDeviceInfo(&devInfo))
        {
            currentValidDevices.push_back(devInfo);
        }
    }

    if(currentValidDevices.size() > m_foundDevices.size() ) // found new device
    {
        devListChanged = addElementsToValidDevices(&currentValidDevices);
    }
    else if(currentValidDevices.size() < m_foundDevices.size()) // disconnected device
    {
        devListChanged = removeElementsFromValidDevices(&currentValidDevices);
    }

    return devListChanged;
}

int UsbDevices::addElementsToValidDevices(QList<UsbDeviceInfo>* refList)
{
    int newDev = 0;
    foreach (UsbDeviceInfo refDevInfo, *refList)
    {
        if(!m_foundDevices.contains(refDevInfo))
        {
            ++newDev;
            m_foundDevices.push_back(refDevInfo);
            QString devInfoMsg = "Found new device: " + refDevInfo.toQString();
            qDebug() << devInfoMsg;
            update_lastDetectedDeviceInfo(devInfoMsg);
        }
    }
    return newDev;
}

int UsbDevices::removeElementsFromValidDevices(QList<UsbDeviceInfo>* refList)
{
    int remDev = 0;
    foreach (UsbDeviceInfo refDevInfo, m_foundDevices)
    {
        if(!refList->contains(refDevInfo))
        {
            ++remDev;
            QString devInfoMsg = "Device disconnected: " + refDevInfo.toQString();
            qDebug() << devInfoMsg;
            m_foundDevices.removeOne(refDevInfo);
            update_lastDetectedDeviceInfo(devInfoMsg);
        }
    }
    return remDev;
}
