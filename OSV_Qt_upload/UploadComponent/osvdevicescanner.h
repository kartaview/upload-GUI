#ifndef DEVICETHREAD_H
#define DEVICETHREAD_H
#include <QTimer>
#include "usbdevices.h"

class OSVDeviceScanner : public QObject
{
    Q_OBJECT
public:
    OSVDeviceScanner();
    void stop();
    void start();
    UsbDevices* getDetectedOsvDevices();
private:
    QTimer m_timer;
    UsbDevices m_osvDevices;
};

#endif // DEVICETHREAD_H
