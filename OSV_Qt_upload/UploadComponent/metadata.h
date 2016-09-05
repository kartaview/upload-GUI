#ifndef METADATA_H
#define METADATA_H

#include <qqmlhelpers.h>
#include <QFile>

class Metadata : public QObject
{
    Q_OBJECT
public:
    explicit Metadata(const QString &filepath, QObject *parent = 0);
    void processVideoMetadata(double &lat, double &lng);

    QString     getPath();
    QString     getDecompressPath();
    long long   getSize();
    QString     getPlatformName();
    QString     getPlatformVersion();

private:
    QString     m_path;
    QString     m_decompressPath;
    QString     m_platformName;
    QString     m_platformVersion;
    long long   m_size;
};

#endif // METADATA_H
