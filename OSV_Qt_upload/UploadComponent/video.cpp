#include "video.h"
#include <QFileInfo>
#include <QDebug>

Video::Video(const QString &filePath, QObject *parent)
    : QObject(parent),
      m_path(filePath),
      m_status(FileStatus::AVAILABLE)
{
    m_size = QFileInfo(filePath).size();
}

bool Video::operator<(const Video & v) const
{
    return this->getId() < v.getId();
}

int Video::getId() const
{
    return QFileInfo(m_path).baseName().toInt();
}

bool Video::lessThan(Video * v1, Video * v2)
{
    return v1->getId() < v2->getId();
}
