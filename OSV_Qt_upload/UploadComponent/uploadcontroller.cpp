#include "uploadcontroller.h"

UploadController::UploadController(LoginController* lc, PersistentController* pc, QObject* parent)
    : QObject(parent)
    , m_loginController(lc)
    , m_persistentController(pc)
    , m_isUploadPaused(false)
    , m_elapsedTimeCounter(new ElapsedTimeCounter())
    , m_OSVAPI(new OSVAPI())
    , m_isUploadComplete(false)
    , m_isError(false)
{
    reset();
    onInformationChanged();

    connect(m_OSVAPI, SIGNAL(errorFound()), this, SLOT(onErrorFound()));
    connect(m_OSVAPI, SIGNAL(sequenceCreated(int)), this, SLOT(onSequenceCreated(int)));
    connect(m_OSVAPI, SIGNAL(SequenceFinished(int)), this, SLOT(onSequenceFinished(int)));
    connect(m_OSVAPI, SIGNAL(photoUploaded(int, int)), this, SLOT(onPhotoUploaded(int, int)));
    connect(m_OSVAPI, SIGNAL(videoUploaded(int, int)), this, SLOT(onVideoUploaded(int, int)));
    connect(m_OSVAPI, SIGNAL(uploadProgress(qint64)), this, SLOT(onUploadProgress(qint64)));
    connect(m_persistentController, SIGNAL(informationChanged()), this,
            SLOT(onInformationChanged()));
    connect(m_elapsedTimeCounter, SIGNAL(elapsedTimeChanged()), this, SLOT(onElapsedTimeChanged()));
}

UploadController::~UploadController()
{
    disconnect(m_OSVAPI, SIGNAL(sequenceCreated(int)), this, SLOT(onSequenceCreated(int)));
    disconnect(m_OSVAPI, SIGNAL(sequenceFinished(int)), this, SLOT(onSequenceFinished(int)));
    disconnect(m_OSVAPI, SIGNAL(photoUploaded(int, int)), this, SLOT(onPhotoUploaded(int, int)));
    disconnect(m_OSVAPI, SIGNAL(videoUploaded(int, int)), this, SLOT(onVideoUploaded(int, int)));
    disconnect(m_OSVAPI, SIGNAL(uploadProgress(qint64)), this, SLOT(onUploadProgress(qint64)));
    disconnect(m_persistentController, SIGNAL(informationChanged()), this,
               SLOT(onInformationChanged()));
    disconnect(m_elapsedTimeCounter, SIGNAL(elapsedTimeChanged()), this,
               SLOT(onElapsedTimeChanged()));
}

void UploadController::startUpload()
{
    m_elapsedTimeCounter->start();
    selectNewSequence();
}

void UploadController::selectNewSequence()
{
    int sequenceIndex = 0;
    while (sequenceIndex < m_persistentController->getPersistentSequences().count() &&
           (m_persistentController->getElement(sequenceIndex)->getSequenceStatus() ==
            SequenceStatus::SUCCESS))
    {
        sequenceIndex++;
    }
    if (sequenceIndex < m_persistentController->getPersistentSequences().count())
    {
        setIsUploadStarted(true);
        UploadSequence(m_persistentController->getElement(sequenceIndex), sequenceIndex);
    }
    else  // here all the upload is finished
    {
        m_elapsedTimeCounter->stop();
        setIsUploadComplete(true);
    }
}

void UploadController::UploadSequence(PersistentSequence* sequence, const int sequenceIndex)
{
    sequence->setToken(m_loginController->getClientToken());
    const int photoCount(sequence->getPhotos().count());
    const int videoCount(sequence->getVideos().count());

    switch (sequence->getSequenceStatus())
    {
        case SequenceStatus::AVAILABLE:
            m_OSVAPI->requestNewSequence(sequence, sequenceIndex);
            break;
        case SequenceStatus::BUSY:
            for (int index = 0; index < kCountThreads; index++)
            {
                if (photoCount)
                {
                    const int multithreadIndex = sequence->getIndexOfNextAvailablePhoto();
                    if (multithreadIndex != -1)
                    {
                        m_OSVAPI->requestNewPhoto(sequence, sequenceIndex, multithreadIndex);
                    }
                    else if (multithreadIndex == -1 && sequence->areAllFilesSent())
                    {
                        m_OSVAPI->requestSequenceFinished(sequence, sequenceIndex);
                        break;
                    }
                }

                if (videoCount)
                {
                    const int multithreadIndex = sequence->getIndexOfNextAvailableVideo();
                    if (multithreadIndex != -1)
                    {
                        m_OSVAPI->requestNewVideo(sequence, sequenceIndex, multithreadIndex);
                    }
                    else if (multithreadIndex == -1 && sequence->areAllFilesSent())
                    {
                        m_OSVAPI->requestSequenceFinished(sequence, sequenceIndex);
                        break;
                    }
                }
            }
            break;
        case SequenceStatus::FAILED:  // TODO same functionality as in BUSY, maybe eliminate
                                      // Failed/Busy
            onInformationChanged();
            if (sequence->sequenceId() > -1)
            {
                if (photoCount)
                    m_OSVAPI->requestNewPhoto(sequence, sequenceIndex,
                                              sequence->getIndexOfNextAvailablePhoto());
                else
                    m_OSVAPI->requestNewVideo(sequence, sequenceIndex,
                                              sequence->getIndexOfNextAvailablePhoto());
            }
            else
            {
                m_OSVAPI->requestNewSequence(sequence, sequenceIndex);
            }
            break;
        case SequenceStatus::FAILED_FINISH:
            m_OSVAPI->requestSequenceFinished(sequence, sequenceIndex);
            break;
        default:
            selectNewSequence();
            break;
    }
}

void UploadController::onSequenceCreated(int sequenceIndex)
{
    PersistentSequence* sequence = m_persistentController->getElement(sequenceIndex);
    m_persistentController->updatePersistentObject(sequence);

    if (sequence->getPhotos().size())
    {
        qDebug() << "New photo sequence!";
        for (int index = 0; index < kCountThreads && index < sequence->getPhotos().size(); index++)
        {
            m_OSVAPI->requestNewPhoto(sequence, sequenceIndex, index);
        }
    }
    else if (sequence->getVideos().size())
    {
        qDebug() << "New video sequence!";
        for (int index = 0; index < kCountThreads && index < sequence->getVideos().size(); index++)
        {
            m_OSVAPI->requestNewVideo(sequence, sequenceIndex, index);
        }
    }
}

double UploadController::calculateProgressPercentage()
{
    if (m_persistentController->get_totalSize() == 0 ||
        m_persistentController->get_totalFiles() == 0)
    {
        return 0;
    }

    const double uploadedSizePerc =
        (double)((m_uploadedSize * 100) / (double)m_persistentController->get_totalSize());
    const double serverReplyPerc =
        (m_uploadedNoFiles * 100) / m_persistentController->get_totalFiles();

    return 0.9 * uploadedSizePerc + 0.1 * serverReplyPerc;
}

void UploadController::onUploadProgress(qint64 bytesDiff)
{
    const int timeDiff = m_elapsedTimeCounter->getElapsedTime();
    int       remainingTimeSec;
    qint64    speed;
    setUploadedSize(m_uploadedSize + bytesDiff);
    if (timeDiff > 0)
    {
        speed              = m_uploadedSize / timeDiff;  // Download Speed in B/s
        qint64 remainingUp = m_persistentController->get_totalSize() - m_uploadedSize;
        if (speed)
        {
            remainingTimeSec = remainingUp / speed;
            setUploadSpeed(speed);
            setRemainingTime(remainingTimeSec);
        }
    }

    if (m_uploadedSize > m_persistentController->get_totalSize())
    {
        setUploadedSize(m_persistentController->get_totalSize());
    }

    setPercentage(calculateProgressPercentage());
}

void UploadController::onPhotoUploaded(int sequenceIndex, int photoIndex)
{
    PersistentSequence* sequence = m_persistentController->getElement(sequenceIndex);

    if (!sequence->isFileSent(photoIndex))  // make sure is not duplicated
    {
        sequence->setFileSentOnIndex(photoIndex);
        setUploadedNoFiles(m_uploadedNoFiles + 1);
        m_persistentController->updatePersistentObject(sequence);
    }

    const int nextIndex = sequence->getIndexOfNextAvailablePhoto();
    if (nextIndex != -1)
    {
        m_OSVAPI->requestNewPhoto(sequence, sequenceIndex, nextIndex);
    }
    else if (sequence->areAllFilesSent())
    {
        m_OSVAPI->requestSequenceFinished(sequence, sequenceIndex);
    }
}

void UploadController::onVideoUploaded(int sequenceIndex, int videoIndex)
{
    PersistentSequence* sequence = m_persistentController->getElement(sequenceIndex);

    if (!sequence->isFileSent(videoIndex))  // make sure is not duplicated
    {
        sequence->setFileSentOnIndex(videoIndex);
        setUploadedNoFiles(m_uploadedNoFiles + 1);
        m_persistentController->updatePersistentObject(sequence);
    }

    const int nextIndex = sequence->getIndexOfNextAvailableVideo();
    if (nextIndex != -1)
    {
        m_OSVAPI->requestNewVideo(sequence, sequenceIndex, nextIndex);
    }
    else if (sequence->areAllFilesSent())
    {
        m_OSVAPI->requestSequenceFinished(sequence, sequenceIndex);
    }
}

void UploadController::onSequenceFinished(int sequenceIndex)
{
    qDebug() << "Sequence Finished!";
    m_persistentController->updatePersistentObject(
        m_persistentController->getElement(sequenceIndex));
    selectNewSequence();
}

/*
 *  Function pauses/resumes upload when pressed
 *  If paused, the button's property changes to resume and vice-versa
 */
void UploadController::pauseUpload()
{
    qDebug() << "pause upload";
    setIsUploadPaused(true);
    blockSignals(true);
    m_OSVAPI->pauseUpload();
    m_elapsedTimeCounter->pause();
}

void UploadController::resumeUpload()
{
    qDebug() << "resume upload ";
    blockSignals(false);
    setIsUploadPaused(false);
    m_elapsedTimeCounter->resume();
    m_OSVAPI->resumeUpload();
    m_persistentController->resetStatusForUnsentSequenceFiles();
    onInformationChanged();
    selectNewSequence();
}

void UploadController::onInformationChanged()
{
    int    uploadedFiles = 0;
    qint64 uploadedSize  = 0;
    foreach (PersistentSequence* s, m_persistentController->getPersistentSequences())
    {
        if (s->getSequenceStatus() != SequenceStatus::SUCCESS)
        {
            if (!s->getMetadata()->getPath().isEmpty() &&
                s->getSequenceStatus() != SequenceStatus::AVAILABLE)
            {
                uploadedSize += s->getMetadata()->getSize();
            }

            m_persistentController->updatePersistentObject(s);

            if (s->type() == PersistentSequence::SequenceType::PHOTO)
            {
                const QList<Photo*> photos(s->getPhotos());
                int                 photoIndex = 0;
                while (photoIndex < photos.count())
                {
                    if (s->isFileSent(photoIndex))
                    {
                        Photo* currentPhoto = photos[photoIndex];
                        currentPhoto->setStatus(FileStatus::DONE);
                        ++uploadedFiles;
                        uploadedSize += currentPhoto->getSize();
                    }
                    ++photoIndex;
                }
            }
            else if (s->type() == PersistentSequence::SequenceType::VIDEO)
            {
                const QList<Video*> videos(s->getVideos());
                int                 videoIndex = 0;
                while (videoIndex < videos.count())
                {
                    if (s->isFileSent(videoIndex))
                    {
                        Video* currentVideo = videos[videoIndex];
                        currentVideo->setStatus(FileStatus::DONE);
                        ++uploadedFiles;
                        uploadedSize += currentVideo->getSize();
                    }
                    ++videoIndex;
                }
            }
        }
        else if (s->getSequenceStatus() == SequenceStatus::SUCCESS)
        {
            if (s->type() == PersistentSequence::SequenceType::PHOTO)
            {
                uploadedFiles += s->getPhotos().size();
            }
            else if (s->type() == PersistentSequence::SequenceType::VIDEO)
            {
                uploadedFiles += s->getVideos().size();
            }
            uploadedSize += s->size();
        }
    }

    setUploadedNoFiles(uploadedFiles);
    setUploadedSize(uploadedSize);

    if (m_persistentController->get_totalSize())
    {
        setPercentage(calculateProgressPercentage());
    }
    else
    {
        setPercentage(0);
    }
}

void UploadController::onErrorFound()  // send also a message
{
    setIsError(true);
    m_elapsedTimeCounter->stop();
}

void UploadController::errorAknowledged()
{
    setIsError(false);
    reset();
}

void UploadController::onElapsedTimeChanged()
{
    setElapsedTime(m_elapsedTimeCounter->getElapsedTime());
}

void UploadController::resetUploadValues()
{
    reset();
    m_persistentController->resetProperties();
}

void UploadController::reset()
{
    setIsUploadStarted(false);
    setElapsedTime(0);
    m_elapsedTimeCounter->reset();

    setIsUploadComplete(false);
    setUploadedNoFiles(0);
    setUploadedSize(0);
    setUploadSpeed(0);
    setPercentage(0);
    setRemainingTime(0);
}

// Getters
bool UploadController::isUploadPaused() const
{
    return m_isUploadPaused;
}

int UploadController::remainingTime() const
{
    return m_remainingTime;
}

int UploadController::uploadedNoFiles() const
{
    return m_uploadedNoFiles;
}

long long UploadController::uploadedSize() const
{
    return m_uploadedSize;
}

int UploadController::percentage() const
{
    return m_percentage;
}

long long UploadController::uploadSpeed() const
{
    return m_uploadSpeed;
}

bool UploadController::isUploadStarted() const
{
    return m_isUploadStarted;
}

long long UploadController::elapsedTime() const
{
    return m_elapsedTime;
}

bool UploadController::isError() const
{
    return m_isError;
}

bool UploadController::isUploadComplete() const
{
    return m_isUploadComplete;
}

// Setters
void UploadController::setIsUploadPaused(const bool isUploadPaused)
{
    m_isUploadPaused = isUploadPaused;
    emit isUploadPausedChanged();
}

void UploadController::setRemainingTime(const int remainingTime)
{
    m_remainingTime = remainingTime;
    emit remainingTimeChanged();
}

void UploadController::setUploadedNoFiles(const int uploadedNoFiles)
{
    m_uploadedNoFiles = uploadedNoFiles;
    emit uploadedNoFilesChanged();
}

void UploadController::setUploadedSize(const long long& uploadedSize)
{
    m_uploadedSize = uploadedSize;
    emit uploadedSizeChanged();
}

void UploadController::setPercentage(const int percentage)
{
    m_percentage = percentage;
    emit percentageChanged();
}

void UploadController::setUploadSpeed(const long long& uploadSpeed)
{
    m_uploadSpeed = uploadSpeed;
    emit uploadSpeedChanged();
}

void UploadController::setIsUploadStarted(const bool isUploadStarted)
{
    m_isUploadStarted = isUploadStarted;
    emit isUploadStartedChanged();
}

void UploadController::setElapsedTime(const long long& elapsedTime)
{
    m_elapsedTime = elapsedTime;
    emit elapsedTimeChanged();
}

void UploadController::setIsError(const bool isError)
{
    m_isError = isError;
    emit isErrorChanged();
}

void UploadController::setIsUploadComplete(const bool isUploadComplete)
{
    m_isUploadComplete = isUploadComplete;
    emit isUploadCompleteChanged();
}
