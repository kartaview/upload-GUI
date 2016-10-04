#include "UploadManager.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QDebug>
#include <QHttpMultiPart>
#include <QFileInfo>
#include <QCoreApplication>
#include "metadata.h"

UploadManager::UploadManager(QObject *parent)
    : QObject(parent),
      m_uploadPaused(false)
{
    m_manager = new QNetworkAccessManager();

    //connect fail signal-slots
    connect(this, SIGNAL(NewSequenceFailed(PersistentSequence*,int)), this, SLOT(onNewSequenceFailed(PersistentSequence*,int)), Qt::QueuedConnection);
    connect(this, SIGNAL(SequenceFinishedFailed(PersistentSequence*,int)), this, SLOT(onSequenceFinishedFailed(PersistentSequence*,int)), Qt::QueuedConnection);
    connect(this, SIGNAL(NewPhotoFailed(PersistentSequence*,int, int)), this, SLOT(onNewPhotoFailed(PersistentSequence*,int, int)), Qt::QueuedConnection);
    connect(this, SIGNAL(NewVideoFailed(PersistentSequence*,int, int)), this, SLOT(onNewVideoFailed(PersistentSequence*,int, int)), Qt::QueuedConnection);

}

UploadManager::~UploadManager()
{
    delete m_manager;
}

QJsonObject objectFromString(const QString& in)
{
    QJsonObject obj;

    QJsonDocument doc = QJsonDocument::fromJson(in.toUtf8());

    // check validity of the document
    if(!doc.isNull())
    {
        if(doc.isObject())
        {
            obj = doc.object();
        }
    }

    return obj;
}

void delay()
{
    const QTime dieTime(QTime::currentTime().addSecs(1));
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void UploadManager::requestNewSequence(PersistentSequence *sequence, const int sequenceIndex)
{
    if(m_uploadPaused)
    {
        return;
    }
    HTTPRequest *request = new HTTPRequest(NULL,m_manager);
    connect(request, SIGNAL(newBytesDifference(qint64)), this, SIGNAL(uploadProgress(qint64)));

            request->setHandlerFunc([=] (QNetworkReply *reply)
    {
        bool sequenceFailed = false;
        if (reply && !m_uploadPaused)
        {
            QByteArray data = reply->readAll();
            QString string_data = QString::fromLatin1(data.data());
            QJsonObject json = objectFromString(string_data);
            qDebug()<<string_data;
            QJsonObject statusObj;
            OSVStatusCode statusCode = OSVStatusCode::STATUS_INCORRECT;
            if(!json.isEmpty())
            {
                statusObj = json["status"].toObject();
                statusCode = (OSVStatusCode)statusObj["apiCode"].toString().toInt();
            }

            if (statusCode == OSVStatusCode::SUCCESS)
            {
                QJsonObject osvObj = json["osv"].toObject();
                QJsonObject sequenceObj = osvObj["sequence"].toObject();

                sequence->read(sequenceObj);
                sequence->set_status(SequenceStatus::BUSY);
                disconnect(request, SIGNAL(newBytesDifference(qint64)), this, SIGNAL(uploadProgress(qint64)));
                emit sequenceCreated(sequenceIndex);
            }
            else if (statusCode == OSVStatusCode::BAD_LOGIN)
            {
                emit errorFound();
            }
            else
            {
                sequenceFailed = true;
            }
        }
        else
        {
            sequenceFailed = true;
        }

        if(sequenceFailed)
        {
            emit NewSequenceFailed(sequence, sequenceIndex);
        }

        //delete captured request
        reply->deleteLater();
        delete request;
    });

    QFile *file(nullptr);
    QByteArray buffer(nullptr);
    if(!sequence->get_metadata()->getPath().isEmpty())
    {
        file = new QFile(sequence->get_metadata()->getPath());
        if(!file->open(QIODevice::ReadOnly))
        {
            qDebug()<<"Can not open!";
        }
        else
        {
            buffer = file->read(file->size());
        }
    }

    const QString token(sequence->get_token());

    QHttpMultiPart *map = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart filePart;

    double lat = sequence->getSequence()->get_current_lat();
    double lng = sequence->getSequence()->get_current_lng();

    bool emptyData = false;
    if((buffer.isEmpty() && sequence->get_videos().size()) || sequence->get_token().isEmpty() || !(lat && lng))
    {
        emptyData = true;
    }
    else
    {
        // TO DO : select corect mime-type -> + txt
        if(!buffer.isNull())
        {
            filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-gzip"));
            filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"metaData\"; filename=\""+file->fileName()+"\""));
            filePart.setBody(buffer);
            file->setParent(map);
            map->append(filePart);
            file->close();

            //platform from metadata
            QHttpPart platformNamePart;
            platformNamePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"platformName\""));
            platformNamePart.setBody(sequence->get_metadata()->getPlatformName().toLatin1());
            map->append(platformNamePart);

            QHttpPart platformVersionPart;
            platformVersionPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"platformVersion\""));
            platformVersionPart.setBody(sequence->get_metadata()->getPlatformVersion().toLatin1());
            map->append(platformVersionPart);
        }

        QHttpPart clientTokenPart;
        clientTokenPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"clientToken\""));
        clientTokenPart.setBody(sequence->get_token().toLatin1());
        map->append(clientTokenPart);

        QString coordinate = QString::number(lat)+","+QString::number(lng);
        QHttpPart coordinatePart;
        coordinatePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"currentCoordinate\""));
        coordinatePart.setBody(coordinate.toLatin1());
        map->append(coordinatePart);

        QHttpPart uploadSourcePart;
        const QString uploadSource("Qt tool");
        uploadSourcePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"uploadSource\""));
        uploadSourcePart.setBody(uploadSource.toLatin1());
        map->append(uploadSourcePart);
    }

    const QString url(kProtocol + kBaseProductionUrl + kVersion + kCommandSequence);
    qDebug() << "Request URL: " << url;
    qDebug() << "Metadata: " << sequence->get_metadata()->getPath();
    if(!emptyData)
    {
        request->post(url, map);
    }
    else
    {
        //POP UP
        qDebug()<<"POP UP";
    }
}

void UploadManager::onNewSequenceFailed(PersistentSequence *sequence, const int sequenceIndex)
{
    qDebug()<<"New Sequence Failed!";
    sequence->set_status(SequenceStatus::FAILED);

    delay();
    requestNewSequence(sequence, sequenceIndex);
}

void UploadManager::requestSequenceFinished(PersistentSequence *sequence, const int sequenceIndex) //sequenceId, externalUserId, userType
{
    if(m_uploadPaused)
    {
        return;
    }

    HTTPRequest *request = new HTTPRequest(NULL,m_manager);
    connect(request, SIGNAL(newBytesDifference(qint64)), this, SIGNAL(uploadProgress(qint64)));

    request->setHandlerFunc([=] (QNetworkReply *reply)
    {
        bool sequenceFailed = false;
        if (reply  && !m_uploadPaused)
        {
            QByteArray data = reply->readAll();
            QString string_data = QString::fromLatin1(data.data());
            qDebug()<<string_data;
            QJsonObject json = objectFromString(string_data);

            QJsonObject statusObj;
            OSVStatusCode statusCode = OSVStatusCode::STATUS_INCORRECT;
            if(!json.isEmpty())
            {
                statusObj = json["status"].toObject();
                statusCode = (OSVStatusCode)statusObj["apiCode"].toString().toInt();
            }

            if (statusCode == OSVStatusCode::SUCCESS && sequence->get_status() != SequenceStatus::SUCCESS)
            {
                sequence->set_status(SequenceStatus::SUCCESS);
                disconnect(request, SIGNAL(newBytesDifference(qint64)), this, SIGNAL(uploadProgress(qint64)));
                emit SequenceFinished(sequenceIndex);
            }
            else
            {
                qDebug()<<"Incorrect sequence finished!";
                sequenceFailed = true;
            }

        }
        else
        {
            qDebug()<<"Bad reply! ( sequence finished ) ";
            sequenceFailed = true;

        }

        if(sequenceFailed)
        {
            emit SequenceFinishedFailed(sequence, sequenceIndex);
        }

        reply->deleteLater();
        delete request;
    });

    QMap<QString, QString> map = QMap<QString, QString>();
    bool emptyData = false;

    if(sequence->getSequence()->get_id() < 0 )
    {
        emptyData = true;
    }
    else
    {
        map.insert("sequenceId",QString::number(sequence->getSequence()->get_id()));

    }

    const QString url(kProtocol + kBaseProductionUrl + kVersion + kCommandSequenceFinished);
    qDebug()<<"Request URL: "<<url;
    if(!emptyData)
    {
        request->post(url, map);
    }
    else
    {
        //POP UP
        qDebug()<<"POP UP";
    }
}

void UploadManager::onSequenceFinishedFailed(PersistentSequence *sequence, const int sequenceIndex)
{
    qDebug()<<"Sequence Finish Failed! Bad Reply!";
    sequence->set_status(SequenceStatus::FAILED_FINISH);
    delay();
    requestSequenceFinished(sequence, sequenceIndex);
}

void UploadManager::requestNewPhoto(PersistentSequence *sequence, const int sequenceIndex, const int photoIndex)
{
    const QList<Photo*> photoList = sequence->get_photos();

    if(m_uploadPaused || photoIndex >= photoList.count())
    {
        return;
    }
    Photo* currentPhoto = photoList.at(photoIndex);
    HTTPRequest *request = new HTTPRequest(NULL,m_manager);
    connect(request, SIGNAL(newBytesDifference(qint64)), this, SIGNAL(uploadProgress(qint64)));

    request->setHandlerFunc([=] (QNetworkReply *reply)
    {
        currentPhoto->setStatus(FileStatus::BUSY);

        OSVStatusCode statusCode;
        bool sequenceFailed = false;
        if (reply  && !m_uploadPaused)
        {
            QByteArray data = reply->readAll();
            QString string_data = QString::fromLatin1(data.data());

            QJsonObject json = objectFromString(string_data);
            qDebug()<<string_data;
            QJsonObject statusObj;
            statusCode = OSVStatusCode::STATUS_INCORRECT;
            if(!json.isEmpty())
            {
                statusObj = json["status"].toObject();
                statusCode = (OSVStatusCode)statusObj["apiCode"].toString().toInt();
            }
            if (statusCode == OSVStatusCode::SUCCESS)
            {
                qDebug()<<"Succes, photo index: "<<photoIndex;
                if(currentPhoto)
                {
                    currentPhoto->setStatus(FileStatus::DONE);
                }
                disconnect(request, SIGNAL(newBytesDifference(qint64)), this, SIGNAL(uploadProgress(qint64)));
                emit photoUploaded(sequenceIndex, photoIndex);
            }
            else if(statusCode == OSVStatusCode::DUPLICATE)
            {
                currentPhoto->setStatus(FileStatus::DONE);
                emit photoUploaded(sequenceIndex, photoIndex);
            }
            else
            {
                sequenceFailed = true;
            }

        }
        else
        {
            sequenceFailed = true;
            qDebug()<<"Bad reply! ( New photo ) ";


        }
        if(sequenceFailed)
        {
            emit NewPhotoFailed(sequence, sequenceIndex, photoIndex);
        }

        reply->deleteLater();
        delete request;
    });

    QFile * imageFile(nullptr);
    QByteArray buffer(nullptr);

    imageFile = new QFile(currentPhoto->getPath());
    if(!imageFile->open(QIODevice::ReadOnly))
    {
        qDebug()<<"Can not open image!";
    }
    buffer = imageFile->read(imageFile->size());

    QHttpMultiPart *map = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart imagePart;
    QHttpPart seqIdPart;
    QHttpPart seqIndexPart;
    QHttpPart coordinatePart;
    double lat = currentPhoto->getLat();
    double lng = currentPhoto->getLng();

    bool isEmpty = false;
    int sequenceId = sequence->getSequence()->get_id();

    if(buffer.isEmpty() || sequenceId < 0 || photoIndex < 0 || !(lat && lng))
    {
        isEmpty = true;
    }
    else
    {
        imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
        imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"photo\"; filename=\""+imageFile->fileName()+"\""));
        imagePart.setBody(buffer);
        imageFile->setParent(map);
        map->append(imagePart);
        imageFile->close();

        seqIdPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"sequenceId\""));
        seqIdPart.setBody(QString::number(sequenceId).toLatin1());
        map->append(seqIdPart);

        seqIndexPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"sequenceIndex\""));
        seqIndexPart.setBody(QString::number(photoIndex).toLatin1());
        map->append(seqIndexPart);

        coordinatePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"coordinate\""));
        coordinatePart.setBody((QString::number(lat)+","+QString::number(lng)).toLatin1());
        map->append(coordinatePart);
    }
    const QString url(kProtocol + kBaseProductionUrl + kVersion + kCommandPhoto);
    qDebug()<<"Request URL : "<<url<<" --> PhotoIndex : "<< photoIndex << " | FileName: "<< QFileInfo(currentPhoto->getPath()).baseName()<< " | Coord : "<<lat<< " - "<<lng;

    if(!isEmpty)
    {
        currentPhoto->setStatus(FileStatus::BUSY);
        request->post(url, map);
    }
}

void UploadManager::onNewPhotoFailed(PersistentSequence *sequence, const int sequenceIndex, const int photoIndex)
{
    qDebug()<<"New Photo Failed! Bad Reply! "<<photoIndex;
    delay();
    requestNewPhoto(sequence, sequenceIndex, photoIndex);
}

void UploadManager::requestNewVideo(PersistentSequence *sequence, const int sequenceIndex,  const int videoIndex)
{
    const QList<Video*> videoList = sequence->get_videos();

    if(m_uploadPaused || videoIndex >= videoList.count())
    {
        return;
    }
    Video* currentVideo = videoList.at(videoIndex);
    HTTPRequest *request = new HTTPRequest(NULL,m_manager);
    connect(request, SIGNAL(newBytesDifference(qint64)), this, SIGNAL(uploadProgress(qint64)));

    request->setHandlerFunc([=] (QNetworkReply *reply)
    {
        currentVideo->set_status(FileStatus::BUSY);
        OSVStatusCode statusCode;
        bool sequenceFailed = false;
        if (reply  && !m_uploadPaused)
        {
            QByteArray data = reply->readAll();
            QString string_data = QString::fromLatin1(data.data());
            QJsonObject json = objectFromString(string_data);
            QJsonObject statusObj;
            statusCode = OSVStatusCode::STATUS_INCORRECT;
            if(!json.isEmpty())
            {
                statusObj = json["status"].toObject();
                statusCode = (OSVStatusCode)statusObj["apiCode"].toString().toInt();
            }

            if (statusCode == OSVStatusCode::SUCCESS)
            {
                qDebug() << "Success, video index: " << videoIndex << " | videoPath: " << currentVideo->get_path();
                disconnect(request, SIGNAL(newBytesDifference(qint64)), this, SIGNAL(uploadProgress(qint64)));
                if(videoIndex < videoList.count())
                {
                    currentVideo->set_status(FileStatus::DONE);
                }
                emit videoUploaded(sequenceIndex, videoIndex);
            }
            else if(statusCode == OSVStatusCode::DUPLICATE)
            {
                currentVideo->set_status(FileStatus::DONE);
                emit videoUploaded(sequenceIndex, videoIndex);
            }
            else
            {
                sequenceFailed = true;
            }

        }
        else
        {
            sequenceFailed = true;
        }

        if(sequenceFailed)
        {
            reply->deleteLater();
            emit NewVideoFailed(sequence, sequenceIndex, videoIndex);
        }

        reply->deleteLater();
        delete request;
    });

    QFile * videoFile(nullptr);
    QByteArray buffer(nullptr);

    videoFile = new QFile(currentVideo->get_path());
    if(!videoFile->open(QIODevice::ReadOnly))
    {
        qDebug()<<"Can not open image!";
    }
    else
    {
        buffer = videoFile->read(videoFile->size());
    }


    QHttpMultiPart *map = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart videoPart;
    QHttpPart seqIdPart;
    QHttpPart seqIndexPart;

    bool isEmpty =false;
    int sequenceId = sequence->getSequence()->get_id();

    if(buffer.isEmpty() || sequenceId < 0 || videoIndex < 0)
    {
        isEmpty = true;
    }
    else
    {
        videoPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("video/mp4"));
        videoPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"video\"; filename=\""+videoFile->fileName()+"\""));
        videoPart.setBody(buffer);
        videoFile->setParent(map);
        map->append(videoPart);

        seqIdPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"sequenceId\""));
        seqIdPart.setBody(QString::number(sequenceId).toLatin1());
        map->append(seqIdPart);

        seqIndexPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"sequenceIndex\""));
        seqIndexPart.setBody(QString::number(videoIndex).toLatin1());
        map->append(seqIndexPart);
        videoFile->close();
    }

    const QString url(kProtocol + kBaseProductionUrl + kVersion + kCommandVideo);
    qDebug()<<"Request URL : "<<url<<" SequenceIndex: "<<videoIndex<<" | Video fileName: "<<QFileInfo(currentVideo->get_path()).baseName();
    if(!isEmpty)
    {
        currentVideo->set_status(FileStatus::BUSY);
        request->post(url, map);
    }

    delete videoFile;
}

void UploadManager::onNewVideoFailed(PersistentSequence *sequence, const int sequenceIndex, const int videoIndex)
{
    qDebug()<<"New video Failed! Bad Reply! "<<videoIndex;
    delay();
    requestNewVideo(sequence, sequenceIndex, videoIndex);
}

void UploadManager::pauseUpload()
{
    m_uploadPaused = true;
}

void UploadManager::resumeUpload()
{
    m_uploadPaused = false;
}
