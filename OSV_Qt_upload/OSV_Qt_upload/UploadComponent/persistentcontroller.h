#ifndef PERSISTENTCONTROLLER_H
#define PERSISTENTCONTROLLER_H

#include <QObject>
#include "jsonserializable.h"
#include <QQueue>
#include "persistentsequence.h"
#include "qqmlobjectlistmodel.h"
#include "UploadManager.h"
#include <QDirIterator>

class PersistentController : public QObject
{
    Q_OBJECT
    QML_WRITABLE_PROPERTY (int, totalFiles)
    QML_WRITABLE_PROPERTY (qint64, totalSize)
    QML_OBJMODEL_PROPERTY (PersistentSequence, sequences)
public:
    explicit PersistentController(QObject *parent = 0);

    void addPersistentObject(PersistentSequence *sequence);
    void updatePersistentObject(PersistentSequence *sequence);
    void calculateTotalInformation();
    void reset();

    PersistentSequence*         getElement(const int index);
    QList<PersistentSequence *> getPersistentSequences();

    int  findIndex(PersistentSequence *sequence);
    void resetProperties();
    void resetStatusForUnsentSequenceFiles();

private:
    bool checkMetadata(const QString &path, PersistentSequence * sequence, qint64 &totalSize);

    void checkPhotosExif(const QString &path, PersistentSequence * sequence, qint64 &totalSize);

    void checkVideos(const QString &path, PersistentSequence * sequence, qint64 &totalSize);

    void    checkPaths(const int index = -1);

    QString convertFolderPath(const QString& folderPath);
    bool    folderExist(const QString &filepath);
    void    write(QJsonObject &json);
    void    read(const QJsonObject &json);
    bool    save();
    bool    load();

private:
    QStringList                  m_enteredDirPath;
    UploadManager*               m_uploadManager;
    QList<PersistentSequence *>  m_persistentSequences;
    const QString                m_saveFilePath;

signals:
    void informationChanged();

public slots:
    Q_INVOKABLE void onFileDialogButton(const QVariant &pathReceived);
    Q_INVOKABLE void removeFolders(const QList<QVariant> indexes);
    Q_INVOKABLE void addPreviewPath(const QVariant &pathReceived);
    Q_INVOKABLE void onDropped();
    Q_INVOKABLE void onExitedDropArea();
};

#endif // PERSISTENTCONTROLLER_H
