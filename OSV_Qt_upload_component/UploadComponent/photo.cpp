#include "photo.h"
#include "exif.h"
#include <QFile>
#include <QFileInfo>
#include <QDebug>

Photo::Photo(QObject *parent)
    : QObject(parent),
      m_path(""),
      m_lat(0),
      m_lng(0),
      m_size(0),
      m_status(FileStatus::AVAILABLE)
{
}

bool Photo::processPhoto(const QString &filePath)
{
    QFile file(filePath);

    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Can not open photo for exif!";
        return false;
    }
    const QByteArray buffer = file.readAll();

    easyexif::EXIFInfo result;
    result.parseFrom((const uchar*)buffer.constData(), buffer.size());
    m_lat = result.GeoLocation.Latitude;
    m_lng = result.GeoLocation.Longitude;

    if(!m_lat || !m_lng)
    {
        qDebug() << "Missing GeoLocation Args!";
        return false;
    }
    else
    {
        m_path = filePath;
    }

    m_size = QFileInfo(filePath).size();
    return true;
}

bool Photo::lessThan(Photo * p1, Photo * p2)
{
    return p1->getId() < p2->getId();
}

int Photo::getId()
{
    return QFileInfo(m_path).baseName().toInt();
}

QString Photo::getPath()
{
    return m_path;
}

double Photo::getLat()
{
    return m_lat;
}

double Photo::getLng()
{
    return m_lng;
}

long long Photo::getSize()
{
    return m_size;
}

FileStatus Photo::getStatus()
{
    return m_status;
}

void Photo::setStatus(const FileStatus &status)
{
    m_status = status;
}
