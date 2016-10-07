#ifndef PERSISTENTSEQUENCE_H
#define PERSISTENTSEQUENCE_H

#include <Sequence.h>
#include <QDebug>
#include "video.h"
#include "metadata.h"
enum class SequenceStatus : int {   BUSY = 2,
                                    SUCCESS = 1,
                                    AVAILABLE = 0,
                                    FAILED = -1,
                                    FAILED_FINISH = -2};

class PersistentSequence : public JsonSerializable
{
    Q_OBJECT

    QML_WRITABLE_PROPERTY (Metadata*, metadata)
    QML_WRITABLE_PROPERTY (QString, path)
    QML_WRITABLE_PROPERTY (QString, name)

    QML_WRITABLE_PROPERTY (QList<Photo *>, photos)
    QML_WRITABLE_PROPERTY (QList<Video *>, videos)

    QML_WRITABLE_PROPERTY (long long, size)
    QML_WRITABLE_PROPERTY (SequenceStatus, status)
    QML_WRITABLE_PROPERTY (QString, token)
    QML_WRITABLE_PROPERTY (Sequence*, sequence)

    Q_PROPERTY (SequenceType type READ type)

public:

    explicit PersistentSequence(QObject *parent = 0);
    ~PersistentSequence();

    enum SequenceType {
        NONE,
        PHOTO,
        VIDEO
    };
    Q_ENUM(SequenceType)

    SequenceType type() const
    {
        return m_type;
    }

    Sequence* getSequence();
    void setSequence(Sequence *sequence);

    void addPhotoInfo(const QString &path, const qint64 &totalSize);

    void addVideoInfo(const QString &path, const qint64 &totalSize);

    bool equals(PersistentSequence *sequence);

    using JsonSerializable::read; //compile
    void read(const QJsonObject &jsonObj);

    using JsonSerializable::write;
    void write(QJsonObject &jsonObj);

    bool isFileSent(int index);
    bool areAllFilesSent();
    void setFileSentOnIndex(int index);

    int getIndexOfNextAvailablePhoto();
    int getIndexOfNextAvailableVideo();
    void resetStatusForUnsentFiles();
    void resetInformation();
private:

    void setFolderPathAndName(const QString &folderPath);
    SequenceType m_type;
    QVector<bool>* m_filesSentIndex;
};

#endif // PERSISTENTSEQUENCE_H
