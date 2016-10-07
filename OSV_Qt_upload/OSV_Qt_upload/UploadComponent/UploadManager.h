#ifndef UPLOADMANAGER_H
#define UPLOADMANAGER_H

#include <QObject>
#include <QFile>
#include <QEventLoop>
#include "httprequest.h"
#include "PersistentSequence.h"
#include "uploadcomponentconstants.h"

class UploadManager : public QObject
{
    Q_OBJECT
public:
    explicit UploadManager(QObject *parent = 0);
    ~UploadManager();

    void requestNewSequence(PersistentSequence *sequence, const int sequenceIndex);
    void requestSequenceFinished(PersistentSequence *sequence, const int sequenceIndex);
    void requestNewPhoto(PersistentSequence *sequence, const int sequenceIndex, const int photoIndex);
    void requestNewVideo(PersistentSequence *sequence, const int sequenceIndex,  const int videoIndex);

    void pauseUpload();
    void resumeUpload();
signals:
    void uploadProgress(qint64 bytesDiff);
    void SequenceFinished(int sequenceIndex);
    void photoUploaded(int sequenceIndex, int photoIndex);
    void videoUploaded(int sequenceIndex, int videoIndex);
    void fileUploaded(int sequenceIndex);
    void sequenceCreated(int sequenceIndex);
    void errorFound();

    void NewSequenceFailed(PersistentSequence *sequence, const int sequenceIndex);
    void SequenceFinishedFailed(PersistentSequence *sequence, const int sequenceIndex);
    void NewPhotoFailed(PersistentSequence *sequence, const int sequenceIndex, const int photoIndex);
    void NewVideoFailed(PersistentSequence *sequence, const int sequenceIndex,  const int videoIndex);

public slots:
    void onNewSequenceFailed(PersistentSequence *sequence, const int sequenceIndex);
    void onSequenceFinishedFailed(PersistentSequence *sequence, const int sequenceIndex);
    void onNewPhotoFailed(PersistentSequence *sequence, const int sequenceIndex, const int photoIndex);
    void onNewVideoFailed(PersistentSequence *sequence, const int sequenceIndex,  const int videoIndex);

private:
    QNetworkAccessManager *m_manager;
    bool m_uploadPaused;
};

#endif // UPLOADMANAGER_H
