#ifndef PHOTODETECTION_H
#define PHOTODETECTION_H

#include <QObject>
#include <qqmlhelpers.h>
#include <QVariantMap>

class PhotoDetection : public QObject
{
    Q_OBJECT
    QML_WRITABLE_PROPERTY (int, value)
    QML_WRITABLE_PROPERTY (double, confidence)
    QML_WRITABLE_PROPERTY (QString, pathToRoi)
    QML_WRITABLE_PROPERTY (QString, pathToSignType)
    QML_WRITABLE_PROPERTY (QVariantMap, bbox) //x,y,w,h
    QML_WRITABLE_PROPERTY (int, photo_index)
public:
    explicit PhotoDetection(QObject *parent = 0);

signals:

public slots:
};

#endif // PHOTODETECTION_H
