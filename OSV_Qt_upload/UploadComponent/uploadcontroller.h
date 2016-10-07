#ifndef UPLOADCONTROLLER_H
#define UPLOADCONTROLLER_H

#include "UploadManager.h"
#include "elapsedtimecounter.h"
#include "logincontroller.h"
#include "persistentcontroller.h"
#include "qqmlhelpers.h"

class UploadController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isUploadPaused READ isUploadPaused NOTIFY isUploadPausedChanged)
    Q_PROPERTY(int remainingTime READ remainingTime NOTIFY remainingTimeChanged)
    Q_PROPERTY(int uploadedNoFiles READ uploadedNoFiles NOTIFY uploadedNoFilesChanged)
    Q_PROPERTY(long long uploadedSize READ uploadedSize NOTIFY uploadedSizeChanged)
    Q_PROPERTY(int percentage READ percentage NOTIFY percentageChanged)
    Q_PROPERTY(long long uploadSpeed READ uploadSpeed NOTIFY uploadSpeedChanged)
    Q_PROPERTY(bool isUploadStarted READ isUploadStarted NOTIFY isUploadStartedChanged)
    Q_PROPERTY(long long elapsedTime READ elapsedTime NOTIFY elapsedTimeChanged)
    Q_PROPERTY(bool isError READ isError NOTIFY isErrorChanged)
    Q_PROPERTY(bool isUploadComplete READ isUploadComplete NOTIFY isUploadCompleteChanged)

public:
    explicit UploadController(LoginController* lc, PersistentController* pc, QObject* parent = 0);
    ~UploadController();
    void reset();

    // Getters
    bool      isUploadPaused() const;
    int       remainingTime() const;
    int       uploadedNoFiles() const;
    long long uploadedSize() const;
    int       percentage() const;
    long long uploadSpeed() const;
    bool      isUploadStarted() const;
    long long elapsedTime() const;
    bool      isError() const;
    bool      isUploadComplete() const;

    // Setters
    void setIsUploadPaused(const bool isUploadPaused);
    void setRemainingTime(const int remainingTime);
    void setUploadedNoFiles(const int uploadedNoFiles);
    void setUploadedSize(const long long& uploadedSize);
    void setPercentage(const int percentage);
    void setUploadSpeed(const long long& uploadSpeed);
    void setIsUploadStarted(const bool isUploadStarted);
    void setElapsedTime(const long long& elapsedTime);
    void setIsError(const bool isError);
    void setIsUploadComplete(const bool isUploadComplete);

signals:
    void isUploadPausedChanged();
    void remainingTimeChanged();
    void uploadedNoFilesChanged();
    void uploadedSizeChanged();
    void percentageChanged();
    void uploadSpeedChanged();
    void isUploadStartedChanged();
    void elapsedTimeChanged();
    void isErrorChanged();
    void isUploadCompleteChanged();

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
    void onErrorFound();

    Q_INVOKABLE void startUpload();
    Q_INVOKABLE void resetUploadValues();
    Q_INVOKABLE void errorAknowledged();

private:
    void   selectNewSequence();
    double calculateProgressPercentage();

private:
    bool      m_isUploadPaused;
    int       m_remainingTime;
    int       m_uploadedNoFiles;
    long long m_uploadedSize;
    int       m_percentage;
    long long m_uploadSpeed;
    bool      m_isUploadStarted;
    long long m_elapsedTime;
    bool      m_isError;
    bool      m_isUploadComplete;

    UploadManager*        m_uploadManager;
    LoginController*      m_loginController;
    PersistentController* m_persistentController;
    ElapsedTimeCounter*   m_elapsedTimeCounter;
};

#endif  // UPLOADCONTROLLER_H
