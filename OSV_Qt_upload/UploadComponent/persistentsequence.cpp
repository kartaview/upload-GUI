#include "persistentsequence.h"
#include <QFileInfo>

PersistentSequence::PersistentSequence(QObject* parent)
    : JsonSerializable(parent)
    , m_sequenceId(-1)
    , m_size(0)
    , m_filesNo(0)
    , m_metadata(nullptr)
    , m_path("")
    , m_name("")
    , m_type(SequenceType::NONE)
    , m_status(SequenceStatus::AVAILABLE)
    , m_filesSentIndex(new QVector<bool>())
{
}

PersistentSequence::~PersistentSequence()
{
    qDebug() << "Called";
}

void PersistentSequence::setFolderPathAndName(const QString& folderPath)
{
    setPath(folderPath);
    setName(QFileInfo(folderPath).fileName());
}

void PersistentSequence::addPhotoInfo(const QString& path, const qint64& totalSize)
{
    setFolderPathAndName(path);
    setFilesNo(m_photos.size());
    m_lat = m_photos[0]->getLat();
    m_lng = m_photos[0]->getLng();
    setSize(totalSize);
    m_type = SequenceType::PHOTO;

    if (m_filesSentIndex->isEmpty())
    {
        m_filesSentIndex->reserve(m_photos.size());
        m_filesSentIndex->fill(0, m_photos.size());
    }
}

void PersistentSequence::addVideoInfo(const QString& path, const qint64& totalSize)
{
    setFolderPathAndName(path);
    setFilesNo(m_videos.count());
    setSize(totalSize);
    m_type = SequenceType::VIDEO;

    if (m_filesSentIndex->isEmpty())
    {
        m_filesSentIndex->reserve(m_videos.count());
        m_filesSentIndex->fill(0, m_videos.count());
    }
    double lat(0), lng(0);
    m_metadata->processVideoMetadata(lat, lng);
    m_lat = lat;
    m_lng = lng;
}

bool PersistentSequence::equals(PersistentSequence* sequence)
{
    return this->m_path == sequence->getPath();
}

void PersistentSequence::write(QJsonObject& jsonObj)
{
    jsonObj["id"]     = m_sequenceId;
    jsonObj["path"]   = m_path;
    jsonObj["status"] = (int)m_status;

    if (m_metadata)
    {
        jsonObj["metadata"] = m_metadata->getPath();
    }
    QString strSentIndex;
    foreach (bool index, *m_filesSentIndex)
    {
        strSentIndex += QString::number(index);
    }
    jsonObj["sentIndex"] = strSentIndex;
}

void PersistentSequence::read(const QJsonObject& jsonObj)
{
    if (jsonObj.contains("id"))
    {
        this->setSequenceId(jsonObj["id"].toInt());
        if (m_sequenceId == 0)
        {
            this->setSequenceId(jsonObj["id"].toString().toInt());
        }
    }

    if (jsonObj.contains("path"))
    {
        this->m_path = jsonObj["path"].toString();
    }

    if (jsonObj.contains("status"))
    {
        this->m_status = (SequenceStatus)jsonObj["status"].toInt();
    }

    if (jsonObj.contains("metadata"))
    {
        this->m_metadata = new Metadata(jsonObj["metadata"].toString(), this);
    }

    if (jsonObj.contains("sentIndex"))
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
    for (int index = 0; index < m_photos.size(); ++index)
    {
        if (m_filesSentIndex->at(index) == false &&
            m_photos[index]->getStatus() == FileStatus::AVAILABLE)
        {
            return index;
        }
    }
    return -1;
}

void PersistentSequence::resetStatusForUnsentFiles()
{
    for (int index = 0; index < m_videos.size(); ++index)
    {
        if (m_filesSentIndex->at(index) == false &&
            m_videos[index]->getStatus() == FileStatus::BUSY)
        {
            m_videos[index]->setStatus(FileStatus::AVAILABLE);
        }
    }

    for (int index = 0; index < m_photos.size(); ++index)
    {
        if (m_filesSentIndex->at(index) == false &&
            m_photos[index]->getStatus() == FileStatus::BUSY)
        {
            const FileStatus fileStatus = FileStatus::AVAILABLE;
            m_photos[index]->setStatus(fileStatus);
        }
    }
}

int PersistentSequence::getIndexOfNextAvailableVideo()
{
    for (int index = 0; index < m_videos.size(); ++index)
    {
        if (m_filesSentIndex->at(index) == false &&
            m_videos[index]->getStatus() == FileStatus::AVAILABLE)
        {
            return index;
        }
    }
    return -1;
}

bool PersistentSequence::areAllFilesSent()
{
    return qFind(m_filesSentIndex->begin(), m_filesSentIndex->end(), false) ==
           m_filesSentIndex->end();
}

bool PersistentSequence::isFileSent(int index)
{
    return m_filesSentIndex->at(index);
}

void PersistentSequence::setFileSentOnIndex(int index)
{
    m_filesSentIndex->replace(index, true);
}

void PersistentSequence::resetInformation()
{
    setSize(0);
    setFilesNo(0);
    m_photos.clear();
    m_videos.clear();
}

// Getters
int PersistentSequence::sequenceId() const
{
    return m_sequenceId;
}

QString PersistentSequence::getPath() const
{
    return m_path;
}

QString PersistentSequence::name() const
{
    return m_name;
}

SequenceStatus PersistentSequence::getSequenceStatus() const
{
    return m_status;
}

PersistentSequence::SequenceType PersistentSequence::type() const
{
    return m_type;
}

QString PersistentSequence::getToken() const
{
    return m_token;
}

long long PersistentSequence::size() const
{
    return m_size;
}

int PersistentSequence::filesNo() const
{
    return m_filesNo;
}

float PersistentSequence::getLat() const
{
    return m_lat;
}

float PersistentSequence::getLng() const
{
    return m_lng;
}

QList<Photo*> PersistentSequence::getPhotos() const
{
    return m_photos;
}

QList<Video*> PersistentSequence::getVideos() const
{
    return m_videos;
}

Metadata* PersistentSequence::getMetadata() const
{
    return m_metadata;
}

// Setters
void PersistentSequence::setSequenceId(const int sequenceId)
{
    m_sequenceId = sequenceId;
    emit sequenceIdChanged();
}

void PersistentSequence::setPath(const QString& path)
{
    m_path = path;
}

void PersistentSequence::setName(const QString& name)
{
    m_name = name;
}

void PersistentSequence::setSequenceStatus(const SequenceStatus status)
{
    m_status = status;
}

void PersistentSequence::setToken(const QString& token)
{
    m_token = token;
}

void PersistentSequence::setSize(const long long& size)
{
    m_size = size;
}

void PersistentSequence::setFilesNo(const int filesNo)
{
    m_filesNo = filesNo;
}

void PersistentSequence::setLat(const float& lat)
{
    m_lat = lat;
}

void PersistentSequence::setLng(const float& lng)
{
    m_lng = lng;
}

void PersistentSequence::setPhotos(const QList<Photo*> photos)
{
    m_photos = photos;
}

void PersistentSequence::setVideos(const QList<Video*> videos)
{
    m_videos = videos;
}

void PersistentSequence::setMetadata(Metadata* metadata)
{
    m_metadata = metadata;
}
