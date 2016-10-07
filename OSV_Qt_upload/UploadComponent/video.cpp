#include "video.h"
#include <QDebug>
#include <QFileInfo>

Video::Video(const QString& filePath, QObject* parent)
    : QObject(parent)
    , m_path(filePath)
    , m_size(QFileInfo(filePath).size())
    , m_status(FileStatus::AVAILABLE)
{
}

bool Video::operator<(const Video& v) const
{
    return this->getId() < v.getId();
}

bool Video::lessThan(Video* v1, Video* v2)
{
    return v1->getId() < v2->getId();
}

// Getters
int Video::getId() const
{
    return QFileInfo(m_path).baseName().toInt();
}

QString Video::getPath() const
{
    return m_path;
}

long long Video::getSize() const
{
    return m_size;
}

FileStatus Video::getStatus() const
{
    return m_status;
}

// Setters
void Video::setPath(const QString& path)
{
    m_path = path;
}

void Video::setSize(const long long& size)
{
    m_size = size;
}

void Video::setStatus(const FileStatus status)
{
    m_status = status;
}
