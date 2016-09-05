#include "uploadcontroller.h"

UploadController::UploadController(LoginController* lc, PersistentController* pc,QObject *parent)
    : QObject(parent),
      m_loginController(lc),
      m_persistentController(pc),
      m_isUploadPaused(false),
      m_elapsedTimeCounter(new ElapsedTimeCounter()),
      m_uploadManager(new UploadManager()),
      m_isUploadComplete(false)
{
    reset();
    onInformationChanged();

    connect(m_uploadManager, SIGNAL(sequenceCreated(int)), this, SLOT(onSequenceCreated(int)));
    connect(m_uploadManager, SIGNAL(SequenceFinished(int)), this, SLOT(onSequenceFinished(int)));
    connect(m_uploadManager, SIGNAL(photoUploaded(int,int)), this, SLOT(onPhotoUploaded(int,int)));
    connect(m_uploadManager, SIGNAL(videoUploaded(int,int)), this, SLOT(onVideoUploaded(int,int)));
    connect(m_uploadManager, SIGNAL(uploadProgress(qint64)), this, SLOT(onUploadProgress(qint64)));
    connect(m_persistentController, SIGNAL(informationChanged()), this, SLOT(onInformationChanged()));
    connect(m_elapsedTimeCounter, SIGNAL(elapsedTimeChanged()), this, SLOT(onElapsedTimeChanged()));
}

UploadController::~UploadController()
{
    disconnect(m_uploadManager, SIGNAL(sequenceCreated(int)), this, SLOT(onSequenceCreated(int)));
    disconnect(m_uploadManager, SIGNAL(sequenceFinished(int)), this, SLOT(onSequenceFinished(int)));
    disconnect(m_uploadManager, SIGNAL(photoUploaded(int,int)), this, SLOT(onPhotoUploaded(int,int)));
    disconnect(m_uploadManager, SIGNAL(videoUploaded(int,int)), this, SLOT(onVideoUploaded(int,int)));
    disconnect(m_uploadManager, SIGNAL(uploadProgress(qint64)), this, SLOT(onUploadProgress(qint64)));
    disconnect(m_persistentController, SIGNAL(informationChanged()), this, SLOT(onInformationChanged()));
    disconnect(m_elapsedTimeCounter, SIGNAL(elapsedTimeChanged()), this, SLOT(onElapsedTimeChanged()));
}

void UploadController::startUpload()
{
    m_elapsedTimeCounter->start();
    selectNewSequence();
}

void UploadController::selectNewSequence()
{
    int sequenceIndex = 0;
    while(sequenceIndex < m_persistentController->getPersistentSequences().count() && (m_persistentController->getElement(sequenceIndex)->get_status() == SequenceStatus::SUCCESS))
    {
        sequenceIndex++;
    }
    if(sequenceIndex < m_persistentController->getPersistentSequences().count())
    {
        set_isUploadStarted(true);
        UploadSequence(m_persistentController->getElement(sequenceIndex), sequenceIndex);
    }
    else //here all the upload is finished
    {
        m_elapsedTimeCounter->stop();
        set_isUploadComplete(true);
    }
}

void UploadController::UploadSequence(PersistentSequence* sequence, const int sequenceIndex)
{
    sequence->set_user(m_loginController->getUser());
    const int photoCount(sequence->get_photos().count());
    const int videoCount(sequence->get_videos().count());

    switch(sequence->get_status())
    {
    case SequenceStatus::AVAILABLE:
        m_uploadManager->requestNewSequence(sequence, sequenceIndex);
        break;
    case SequenceStatus::BUSY:
        for(int index = 0; index<kCountThreads;index++)
        {
            if(photoCount)
            {
                const int multithreadIndex = sequence->getIndexOfNextAvailablePhoto();
                if(multithreadIndex != -1)
                {
                    m_uploadManager->requestNewPhoto(sequence, sequenceIndex, multithreadIndex);
                }
                else if(multithreadIndex == -1 && sequence->areAllFilesSent())
                {
                    m_uploadManager->requestSequenceFinished(sequence, sequenceIndex);
                    break;
                }

            }

            if(videoCount)
            {
                const int multithreadIndex = sequence->getIndexOfNextAvailableVideo();
                if(multithreadIndex != -1)
                {
                    m_uploadManager->requestNewVideo(sequence, sequenceIndex, multithreadIndex);
                }
                else if(multithreadIndex == -1 && sequence->areAllFilesSent())
                {
                    m_uploadManager->requestSequenceFinished(sequence, sequenceIndex);
                    break;
                }
            }

        }
        break;
    case SequenceStatus::FAILED: //TODO same functionality as in BUSY, maybe eliminate Failed/Busy
        onInformationChanged();
        if(sequence->get_sequence()->get_id()>-1)
        {
            if(photoCount)
                m_uploadManager->requestNewPhoto(sequence, sequenceIndex, sequence->getIndexOfNextAvailablePhoto());
            else
                m_uploadManager->requestNewVideo(sequence, sequenceIndex, sequence->getIndexOfNextAvailablePhoto());
        }
        else
        {
            m_uploadManager->requestNewSequence(sequence, sequenceIndex);
        }
        break;
    case SequenceStatus::FAILED_FINISH:
        m_uploadManager->requestSequenceFinished(sequence, sequenceIndex);
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

    if(sequence->get_photos().size())
    {
        qDebug() << "New photo sequence!";
        for(int index = 0; index < kCountThreads && index < sequence->get_photos().size();index++)
        {
            m_uploadManager->requestNewPhoto(sequence, sequenceIndex, index);
        }
    }
    else if(sequence->get_videos().size())
    {
        qDebug()<<"New video sequence!";
        for(int index=0; index<kCountThreads && index < sequence->get_videos().size(); index++)
        {
                m_uploadManager->requestNewVideo(sequence, sequenceIndex, index);
        }
    }

}

double UploadController::calculateProgressPercentage()
{
    if(m_persistentController->get_totalSize() == 0 || m_persistentController->get_totalFiles() == 0)
    {
        return 0;
    }

    const double uploadedSizePerc = (double)((m_uploadedSize * 100) / (double)m_persistentController->get_totalSize());
    const double serverReplyPerc = (m_uploadedNoFiles * 100) / m_persistentController->get_totalFiles();

    return 0.9 * uploadedSizePerc + 0.1 * serverReplyPerc;
}

void UploadController::onUploadProgress(qint64 bytesDiff)
{
    const int timeDiff = m_elapsedTimeCounter->getElapsedTime();
    int remainingTimeSec;
    qint64 speed;
    set_uploadedSize(m_uploadedSize+bytesDiff);
    if(timeDiff > 0)
    {
        speed = m_uploadedSize/timeDiff; //Download Speed in B/s
        qint64 remainingUp = m_persistentController->get_totalSize() - m_uploadedSize;
        if(speed)
        {
            remainingTimeSec = remainingUp/speed;
            set_uploadSpeed(speed);
            set_remainingTime(remainingTimeSec);
        }
    }

    if(m_uploadedSize > m_persistentController->get_totalSize())
    {
        set_uploadedSize(m_persistentController->get_totalSize());
    }

    set_percentage(calculateProgressPercentage());
}

void UploadController::onPhotoUploaded(int sequenceIndex, int photoIndex)
{
    PersistentSequence* sequence = m_persistentController->getElement(sequenceIndex);
    QList<Photo *> photos = sequence->get_photos();

    if(!sequence->isFileSent(photoIndex)) // make sure is not duplicated
    {
        sequence->setFileSentOnIndex(photoIndex);
        set_uploadedNoFiles(m_uploadedNoFiles+1);
        m_persistentController->updatePersistentObject(sequence);
    }

    const int nextIndex = sequence->getIndexOfNextAvailablePhoto();
    if(nextIndex != -1)
    {
        m_uploadManager->requestNewPhoto(sequence, sequenceIndex, nextIndex);
    }
    else if(sequence->areAllFilesSent())
    {
        m_uploadManager->requestSequenceFinished(sequence, sequenceIndex);
    }
}

void UploadController::onVideoUploaded(int sequenceIndex, int videoIndex)
{
    PersistentSequence* sequence = m_persistentController->getElement(sequenceIndex);

    if(!sequence->isFileSent(videoIndex)) // make sure is not duplicated
    {
        sequence->setFileSentOnIndex(videoIndex);
        set_uploadedNoFiles(m_uploadedNoFiles+1);
        m_persistentController->updatePersistentObject(sequence);
    }

    const int nextIndex = sequence->getIndexOfNextAvailableVideo();
    if(nextIndex != -1)
    {
        m_uploadManager->requestNewVideo(sequence, sequenceIndex, nextIndex);
    }
    else if(sequence->areAllFilesSent())
    {
        m_uploadManager->requestSequenceFinished(sequence, sequenceIndex);
    }
}

void UploadController::onSequenceFinished(int sequenceIndex)
{
    qDebug()<<"Sequence Finished!";
    m_persistentController->updatePersistentObject(m_persistentController->getElement(sequenceIndex));
    selectNewSequence();
}

/*
 *  Function pauses/resumes upload when pressed
 *  If paused, the button's property changes to resume and vice-versa
 */
void UploadController::pauseUpload()
{
    qDebug() << "pause upload";
    set_isUploadPaused(true);
    blockSignals(true);
    m_uploadManager->pauseUpload();
    m_elapsedTimeCounter->pause();
}

void UploadController::resumeUpload()
{
    qDebug() << "resume upload ";
    blockSignals(false);
    set_isUploadPaused(false);
    m_elapsedTimeCounter->resume();
    m_uploadManager->resumeUpload();
    m_persistentController->resetStatusForUnsentSequenceFiles();
    onInformationChanged();
    selectNewSequence();
}

void UploadController::onInformationChanged()
{
    int uploadedFiles = 0;
    qint64 uploadedSize = 0;
    foreach(PersistentSequence* s, m_persistentController->getPersistentSequences())
    {
        if(s->get_status() != SequenceStatus::SUCCESS)
        {
            if(!s->get_metadata()->getPath().isEmpty() && s->get_status() != SequenceStatus::AVAILABLE)
            {
                uploadedSize += s->get_metadata()->getSize();
            }

            m_persistentController->updatePersistentObject(s);

            if(s->type() == PersistentSequence::SequenceType::PHOTO)
            {
                const QList<Photo*> photos(s->get_photos());
                int photoIndex = 0;
                while(photoIndex < photos.count())
                {
                    if(s->isFileSent(photoIndex))
                    {
                        Photo* currentPhoto = photos[photoIndex];
                        currentPhoto->setStatus(FileStatus::DONE);
                        ++uploadedFiles;
                        uploadedSize += currentPhoto->getSize();
                    }
                    ++photoIndex;
                }
            }
            else if(s->type() == PersistentSequence::SequenceType::VIDEO)
            {
                const QList<Video*> videos(s->get_videos());
                int videoIndex = 0;
                while(videoIndex < videos.count())
                {
                    if(s->isFileSent(videoIndex))
                    {
                        Video* currentVideo = videos[videoIndex];
                        currentVideo->set_status(FileStatus::DONE);
                        ++uploadedFiles;
                        uploadedSize += currentVideo->get_size();
                    }
                    ++videoIndex;
                }
            }
        }
        else if(s->get_status() == SequenceStatus::SUCCESS)
        {
            if(s->type() == PersistentSequence::SequenceType::PHOTO)
            {
                uploadedFiles += s->get_photos().size();
            }
            else if(s->type() == PersistentSequence::SequenceType::VIDEO)
            {
                uploadedFiles += s->get_videos().size();
            }
            uploadedSize += s->get_size();
        }
    }

    set_uploadedNoFiles(uploadedFiles);
    set_uploadedSize(uploadedSize);

    if(m_persistentController->get_totalSize())
    {
        set_percentage(calculateProgressPercentage());
    }
    else
    {
        set_percentage(0);
    }
}


void UploadController::onElapsedTimeChanged()
{
    set_elapsedTime(m_elapsedTimeCounter->getElapsedTime());
}

void UploadController::resetUploadValues()
{
    reset();
    m_persistentController->resetProperties();
}

void UploadController::reset()
{
    set_isUploadStarted(false);
    set_elapsedTime(0);
    m_elapsedTimeCounter->reset();

    set_isUploadComplete(false);
    set_uploadedNoFiles(0);
    set_uploadedSize(0);
    set_uploadSpeed(0);
    set_percentage(0);
    set_remainingTime(0);
}
