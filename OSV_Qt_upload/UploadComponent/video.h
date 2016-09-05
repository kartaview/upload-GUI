#ifndef VIDEO_H
#define VIDEO_H

#include <qqmlhelpers.h>
#include <uploadcomponentconstants.h>

class Video : public QObject
{
    Q_OBJECT
    QML_WRITABLE_PROPERTY (QString, path)
    QML_WRITABLE_PROPERTY (qint64, size)
    QML_WRITABLE_PROPERTY (FileStatus, status)
public:
    explicit Video(const QString &filePath, QObject *parent = 0);
    bool operator<(const Video & v) const;
    int getId() const;
    static bool lessThan(Video * v1, Video * v2);
signals:

public slots:


};

#endif // VIDEO_H
