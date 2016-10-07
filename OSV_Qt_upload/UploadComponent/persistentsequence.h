#ifndef PERSISTENTSEQUENCE_H
#define PERSISTENTSEQUENCE_H

#include "jsonserializable.h"
#include "metadata.h"
#include "photo.h"
#include "video.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>

enum class SequenceStatus : int
{
    BUSY          = 2,
    SUCCESS       = 1,
    AVAILABLE     = 0,
    FAILED        = -1,
    FAILED_FINISH = -2
};

class PersistentSequence : public JsonSerializable
{
    Q_OBJECT

    Q_PROPERTY(int sequenceId READ sequenceId NOTIFY sequenceIdChanged)
    Q_PROPERTY(SequenceType type READ type)
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(long long size READ size)
    Q_PROPERTY(int filesNo READ filesNo)

public:
    explicit PersistentSequence(QObject* parent = 0);
    ~PersistentSequence();

    enum SequenceType
    {
        NONE,
        PHOTO,
        VIDEO
    };
    Q_ENUM(SequenceType)

    void addPhotoInfo(const QString& path, const qint64& totalSize);
    void addVideoInfo(const QString& path, const qint64& totalSize);
    bool equals(PersistentSequence* sequence);

    bool isFileSent(int index);
    bool areAllFilesSent();
    void setFileSentOnIndex(int index);

    int  getIndexOfNextAvailablePhoto();
    int  getIndexOfNextAvailableVideo();
    void resetStatusForUnsentFiles();
    void resetInformation();

    using JsonSerializable::read;
    void read(const QJsonObject& jsonObj);

    using JsonSerializable::write;
    void write(QJsonObject& jsonObj);

    // Getters
    int            sequenceId() const;
    QString        getPath() const;
    QString        name() const;
    SequenceStatus getSequenceStatus() const;
    SequenceType   type() const;
    QString        getToken() const;
    long long      size() const;
    int            filesNo() const;
    float          getLat() const;
    float          getLng() const;
    QList<Photo*>  getPhotos() const;
    QList<Video*>  getVideos() const;
    Metadata*      getMetadata() const;

    // Setters
    void setSequenceId(const int sequenceId);
    void setPath(const QString& path);
    void setName(const QString& name);
    void setSequenceStatus(const SequenceStatus status);
    void setType(const SequenceType type);
    void setToken(const QString& token);
    void setSize(const long long& size);
    void setFilesNo(const int filesNo);
    void setLat(const float& lat);
    void setLng(const float& lng);
    void setPhotos(const QList<Photo*> photos);
    void setVideos(const QList<Video*> videos);
    void setMetadata(Metadata* metadata);

signals:
    void sequenceIdChanged();

private:
    void setFolderPathAndName(const QString& folderPath);
    QVector<bool>* m_filesSentIndex;

    int            m_sequenceId;
    QString        m_path;
    QString        m_name;
    SequenceStatus m_status;
    SequenceType   m_type;
    long long      m_size;
    QString        m_token;
    int            m_filesNo;
    float          m_lat;
    float          m_lng;
    QList<Photo*>  m_photos;
    QList<Video*>  m_videos;
    Metadata*      m_metadata;
};

#endif  // PERSISTENTSEQUENCE_H
