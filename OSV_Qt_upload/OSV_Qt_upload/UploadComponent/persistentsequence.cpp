#include "persistentsequence.h"
#include <QFileInfo>

PersistentSequence::PersistentSequence(QObject *parent)
    : JsonSerializable(parent),
      m_sequence(new Sequence()),
      m_metadata(nullptr),
      m_path(""),
      m_name(""),
      m_size(0),
      m_type(SequenceType::NONE),
      m_status(SequenceStatus::AVAILABLE),
      m_filesSentIndex(new QVector<bool>())
{
    m_sequence->set_id(-1);
}

PersistentSequence::~PersistentSequence()
{
    qDebug()<<"Called";
}

void PersistentSequence::setFolderPathAndName(const QString& folderPath)
{
    set_path(folderPath);
    set_name(QFileInfo(folderPath).fileName());
}

void PersistentSequence::addPhotoInfo(const QString &path, const qint64 &totalSize)
{
    setFolderPathAndName(path);
    m_sequence->set_photo_no(m_photos.size());
    m_sequence->set_current_lat(m_photos[0]->getLat());
    m_sequence->set_current_lng(m_photos[0]->getLng());
    set_size(totalSize);
    m_type = SequenceType::PHOTO;

    if (m_filesSentIndex->isEmpty())
    {
        m_filesSentIndex->reserve(m_photos.size());
        m_filesSentIndex->fill(0, m_photos.size());
    }
}

void PersistentSequence::addVideoInfo( const QString &path, const qint64 &totalSize)
{
    setFolderPathAndName(path);
    m_sequence->set_photo_no(m_videos.count());
    set_size(totalSize);
    m_type = SequenceType::VIDEO;

    if (m_filesSentIndex->isEmpty())
    {
        m_filesSentIndex->reserve(m_videos.count());
        m_filesSentIndex->fill(0, m_videos.count());
    }
    double lat(0), lng(0);
    m_metadata->processVideoMetadata(lat, lng);
    m_sequence->set_current_lat(lat);
    m_sequence->set_current_lng(lng);
}

bool PersistentSequence::equals(PersistentSequence *sequence)
{
    return this->m_path == sequence->get_path();
}

Sequence* PersistentSequence::getSequence()
{
    return m_sequence;
}

void PersistentSequence::setSequence(Sequence *sequence)
{
    m_sequence = sequence;
}

void PersistentSequence::write(QJsonObject &jsonObj)
{
    jsonObj["id"] = m_sequence->get_id();
    jsonObj["path"] = m_path;
    jsonObj["status"] = (int)m_status;

    if(m_metadata)
    {
        jsonObj["metadata"] = m_metadata->getPath();
    }
    QString strSentIndex;
    foreach(bool index, *m_filesSentIndex)
    {
        strSentIndex += QString::number(index);
    }
    jsonObj["sentIndex"] = strSentIndex;
}

void PersistentSequence::read(const QJsonObject &jsonObj)
{
    if (jsonObj.contains("id"))
    {
        this->m_sequence->set_id(jsonObj["id"].toInt());
        if(m_sequence->get_id() == 0)
        {
            this->m_sequence->set_id(jsonObj["id"].toString().toInt());
        }
    }

    if (jsonObj.contains("path"))
    {
        this->m_path = jsonObj["path"].toString();
    }

    if(jsonObj.contains("status"))
    {
        set_status((SequenceStatus)jsonObj["status"].toInt());
    }

    if(jsonObj.contains("metadata"))
    {
        this->m_metadata = new Metadata(jsonObj["metadata"].toString(), this);
    }

    if(jsonObj.contains("sentIndex"))
    {
        m_filesSentIndex->clear();
        const QString strSentIndex(jsonObj["sentIndex"].toString());
        foreach (QChar sentIndex, strSentIndex)
        {
            m_filesSentIndex->append(sentIndex.digitValue());
        }
    }
}

int PersistentSequence::getIndexOfNextAvailablePhoto()
{
    for(int index = 0; index < m_photos.size(); ++index)
    {
        if(m_filesSentIndex->at(index) == false && m_photos[index]->getStatus() == FileStatus::AVAILABLE)
        {
            return index;
        }
    }
    return -1;
}

void PersistentSequence::resetStatusForUnsentFiles()
{
    for(int index = 0; index < m_videos.size(); ++index)
    {
        if(m_filesSentIndex->at(index) == false && m_videos[index]->get_status() == FileStatus::BUSY)
        {
            m_videos[index]->set_status(FileStatus::AVAILABLE);
        }
    }

    for(int index = 0; index < m_photos.size(); ++index)
    {
        if(m_filesSentIndex->at(index) == false && m_photos[index]->getStatus() == FileStatus::BUSY)
        {
            const FileStatus fileStatus = FileStatus::AVAILABLE;
            m_photos[index]->setStatus(fileStatus);
        }
    }
}

int PersistentSequence::getIndexOfNextAvailableVideo()
{
    for(int index = 0; index < m_videos.size(); ++index)
    {
        if(m_filesSentIndex->at(index) == false && m_videos[index]->get_status() == FileStatus::AVAILABLE)
        {
            return index;
        }
    }
    return -1;
}

bool PersistentSequence::areAllFilesSent()
{
    return qFind(m_filesSentIndex->begin(), m_filesSentIndex->end(), false) == m_filesSentIndex->end();
}

bool PersistentSequence::isFileSent(int index)
{
    return m_filesSentIndex->at(index);
}

void PersistentSequence::setFileSentOnIndex(int index)
{
    m_filesSentIndex->replace(index,true);
}

void PersistentSequence::resetInformation()
{
    set_size(0);
    m_sequence->set_photo_no(0);
    m_photos.clear();
    m_videos.clear();
}
