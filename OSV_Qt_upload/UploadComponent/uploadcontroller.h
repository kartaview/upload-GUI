#ifndef UPLOADCONTROLLER_H
#define UPLOADCONTROLLER_H

#include "qqmlhelpers.h"
#include "UploadManager.h"
#include "logincontroller.h"
#include "persistentcontroller.h"
#include "elapsedtimecounter.h"

class UploadController : public QObject
{
    Q_OBJECT
    QML_WRITABLE_PROPERTY (bool, isUploadPaused)
    QML_WRITABLE_PROPERTY (int, remainingTime)
    QML_WRITABLE_PROPERTY (int, uploadedNoFiles)
    QML_WRITABLE_PROPERTY (qint64, uploadedSize)
    QML_WRITABLE_PROPERTY (int, percentage)
    QML_WRITABLE_PROPERTY (qint64, uploadSpeed)
    QML_WRITABLE_PROPERTY (bool, isUploadStarted)
    QML_WRITABLE_PROPERTY (qint64, elapsedTime)

    QML_WRITABLE_PROPERTY(bool, isUploadComplete)

public:
    explicit UploadController(LoginController* lc, PersistentController* pc, QObject *parent = 0);
    ~UploadController();
    void reset();

public slots:
    void UploadSequence(PersistentSequence* sequence, const int sequenceIndex);
    void onUploadProgress(qint64 bytesDiff);
    void pauseUpload();
    void resumeUpload();
    void onSequenceCreated(int sequenceIndex);
    void onPhotoUploaded(int sequenceIndex, int photoIndex);
    void onVideoUploaded(int sequenceIndex, int videoIndex);
    void onSequenceFinished(int sequenceIndex);
    void onInformationChanged();
    void onElapsedTimeChanged();

    Q_INVOKABLE void startUpload();
    Q_INVOKABLE void resetUploadValues();

private:
    void selectNewSequence();
    double calculateProgressPercentage();

    UploadManager*          m_uploadManager;
    LoginController*        m_loginController;
    PersistentController*   m_persistentController;
    ElapsedTimeCounter*     m_elapsedTimeCounter;
};

#endif // UPLOADCONTROLLER_H
