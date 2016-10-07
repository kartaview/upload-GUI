#ifndef PERSISTENTSEQUENCE_H
#define PERSISTENTSEQUENCE_H

#include "metadata.h"
#include "video.h"
#include <QDebug>
#include <Sequence.h>
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

    QML_WRITABLE_PROPERTY(Metadata*, metadata)
    QML_WRITABLE_PROPERTY(QString, name)

    QML_WRITABLE_PROPERTY(QList<Photo*>, photos)
    QML_WRITABLE_PROPERTY(QList<Video*>, videos)
    QML_WRITABLE_PROPERTY(Sequence*, sequence)

    Q_PROPERTY(SequenceType type READ type)
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(long long size READ size)

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

    using JsonSerializable::read;  // compile
    void read(const QJsonObject& jsonObj);

    using JsonSerializable::write;
    void write(QJsonObject& jsonObj);

    bool isFileSent(int index);
    bool areAllFilesSent();
    void setFileSentOnIndex(int index);

    int  getIndexOfNextAvailablePhoto();
    int  getIndexOfNextAvailableVideo();
    void resetStatusForUnsentFiles();
    void resetInformation();

    // Getters
    QString        getPath() const;
    QString        name() const;
    SequenceStatus getSequenceStatus() const;
    SequenceType   type() const;
    Sequence*      getSequence() const;
    QString        getToken() const;
    long long      size() const;

    // Setters
    void setPath(const QString& path);
    void setName(const QString& name);
    void setSequenceStatus(const SequenceStatus status);
    void setType(const SequenceType type);
    void setSequence(Sequence* sequence);
    void setToken(const QString& token);
    void setSize(const long long& size);

private:
    void setFolderPathAndName(const QString& folderPath);
    SequenceType   m_type;
    QVector<bool>* m_filesSentIndex;

    QString        m_path;
    SequenceStatus m_status;
    long long      m_size;
    QString        m_token;
};

#endif  // PERSISTENTSEQUENCE_H
