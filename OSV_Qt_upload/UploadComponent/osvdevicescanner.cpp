#include "osvdevicescanner.h"
#include <QDebug>

const int scanningCycleDuration = 2000;

OSVDeviceScanner::OSVDeviceScanner()
{
    m_osvDevices.addFilter(libusb_class_code::LIBUSB_CLASS_PTP);
    m_osvDevices.addFilter(libusb_class_code::LIBUSB_CLASS_MASS_STORAGE);

    QTimer::connect(&m_timer, &QTimer::timeout, &m_osvDevices, &UsbDevices::scanUsbDevices);
}

void OSVDeviceScanner::start()
{
    m_timer.start(scanningCycleDuration);
}

void OSVDeviceScanner::stop()
{
    m_timer.stop();
}

UsbDevices* OSVDeviceScanner::getDetectedOsvDevices()
{
    return &m_osvDevices;
}
