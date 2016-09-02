#include "skosvapimanager.h"
#include "skosvapirequestbuilder.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>

SKOSVAPIManager::SKOSVAPIManager(QObject *parent) : QObject(parent)
{
    m_manager = new QNetworkAccessManager();
}

SKOSVAPIManager::~SKOSVAPIManager() {
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
        else
        {
            qDebug() << "Document is not an object" << endl;
        }
    }
    else
    {
        qDebug() << "Invalid JSON...\n" << in << endl;
    }

    return obj;
}

void SKOSVAPIManager::getEcho(std::function<void (const int revisionNo)> success_function, std::function<void ()> failed_function) {
    HTTPRequest *request = new HTTPRequest(NULL,m_manager);

    request->setHandlerFunc([=] (QNetworkReply *reply) {
        if (reply) {
            QByteArray data = reply->readAll();
            QString string_data = QString::fromLatin1(data.data());

            QJsonObject json = objectFromString(string_data);

            QJsonObject statusObj = json["status"].toObject();

            OSVStatusCode statusCode = (OSVStatusCode)statusObj["apiCode"].toString().toInt();

            if (statusCode == OSVStatusCode::SUCCESS) {
                QJsonObject osvObj = json["osv"].toObject();

                int revNo = osvObj["revisionNo"].toString().toInt();

                if (success_function) {
                    success_function(revNo);
                }
            }
            else {
                if (failed_function) {
                    failed_function();
                }
            }
        }
        else {
            if (failed_function) {
                failed_function();
            }
        }

        //delete captured request
        delete request;
    });

    const std::string echoURL = SKOSVAPIRequestBuilder::echoURL();
    QString url = QString::fromStdString(echoURL);

    request->post(url,NULL);
}

void SKOSVAPIManager::getList(std::function<void (QList<Sequence*> sequences, int totalPages)> success_function, std::function<void ()> failed_function,
                              const QString &username,
                              const QString &location,
                              const QString &startDate,
                              const QString &endDate,
                              const int &page,
                              const int &ipp,
                              const int &obdInfo,
                              const QString &platformName,
                              const BoundingBox &bbox) {

    HTTPRequest *request = new HTTPRequest(NULL,m_manager);

    request->setHandlerFunc([=] (QNetworkReply *reply) {
        if (reply) {
            QByteArray data = reply->readAll();
            QString string_data = QString::fromLatin1(data.data());

            QJsonObject json = objectFromString(string_data);

            QJsonObject statusObj = json["status"].toObject();

            OSVStatusCode statusCode = (OSVStatusCode)statusObj["apiCode"].toString().toInt();

            if (statusCode == OSVStatusCode::SUCCESS) {
                QJsonValue items = json.value("currentPageItems");
                QJsonArray array = items.toArray();

                int totalItems = 0;
                foreach (const QJsonValue &v, json.value("totalFilteredItems").toArray()) {
                    QJsonObject object = v.toObject();
                    totalItems = v.toString().toInt();
                    break;
                }
                QList<Sequence*> sequences;

                foreach (const QJsonValue &v, array) {
                    QJsonObject object = v.toObject();

                    Sequence *sequence = new Sequence;
                    sequence->read(object);

                    sequences.append(sequence);
                }

                if (success_function) {
                    success_function(sequences,totalItems);
                }
            }
            else {
                if (failed_function) {
                    failed_function();
                }
            }
        }
        else {
            if (failed_function) {
                failed_function();
            }
        }

        //delete captured request
        delete request;
    });

    const std::string listURL = SKOSVAPIRequestBuilder::listURL();
    QString url = QString::fromStdString(listURL);

    QMap<QString, QString> map = QMap<QString, QString>();
    if (username != NULL) {
        map.insert("username",username);
    }
    if (location != NULL) {
        map.insert("location",location);
    }
    if (startDate != NULL) {
        map.insert("startDate",startDate);
    }
    if (endDate != NULL) {
        map.insert("endDate",endDate);
    }

    map.insert("obdInfo",QString::number(obdInfo));

    if (platformName != NULL) {
        map.insert("platformName",platformName);
    }

    if (bbox.topLeft.latitude != 0.0f && bbox.topLeft.longitude != 0.0f && bbox.bottomRight.latitude != 0.0f && bbox.bottomRight.longitude != 0.0f) {
        map.insert("bbTopLeft",QString::number(bbox.topLeft.latitude) + "," + QString::number(bbox.topLeft.longitude));
        map.insert("bbBottomRight",QString::number(bbox.bottomRight.latitude) + "," + QString::number(bbox.bottomRight.longitude));
    }

    map.insert("page",QString::number(page));
    map.insert("ipp",QString::number(ipp));

    request->post(url,map);
}

void SKOSVAPIManager::getPhotoList(std::function<void (QList<Photo*> photos)> success_function, std::function<void ()> failed_function,
             const QString &sequenceId) {
    HTTPRequest *request = new HTTPRequest(NULL,m_manager);

    request->setHandlerFunc([=] (QNetworkReply *reply) {
        if (reply) {
            QByteArray data = reply->readAll();
            QString string_data = QString::fromLatin1(data.data());

            QJsonObject json = objectFromString(string_data);

            QJsonObject statusObj = json["status"].toObject();

            OSVStatusCode statusCode = (OSVStatusCode)statusObj["apiCode"].toString().toInt();

            if (statusCode == OSVStatusCode::SUCCESS) {
                QJsonObject osvObj = json["osv"].toObject();

                QJsonValue items = osvObj.value("photos");
                QJsonArray array = items.toArray();

                QList<Photo*> photos;

                foreach (const QJsonValue &v, array) {
                    QJsonObject object = v.toObject();

                    Photo *photo = new Photo;

                    photo->read(object);

                    photos.append(photo);
                }

                if (success_function) {
                    success_function(photos);
                }
            }
            else {
                if (failed_function) {
                    failed_function();
                }
            }
        }
        else {
            if (failed_function) {
                failed_function();
            }
        }

        //delete captured request
        delete request;
    });

    const std::string photoURL = SKOSVAPIRequestBuilder::photoListURL();
    QString url = QString::fromStdString(photoURL);

    QMap<QString, QString> map = QMap<QString, QString>();
    if (sequenceId != NULL) {
        map.insert("sequenceId",sequenceId);
    }

    request->post(url,map);
}

void SKOSVAPIManager::getPhoto(std::function<void (QImage image)> success_function,
             const QString &path) {
    HTTPRequest *request = new HTTPRequest(NULL,m_manager);

    request->setHandlerFunc([=] (QNetworkReply *reply) {
        if (reply) {

            //to do, error handling, check http error codes
            QImage image;
            image.loadFromData(reply->readAll());

            if (success_function) {
                success_function(image);
            }
        }

        //delete captured request
        delete request;
    });

    const std::string photoURL = SKOSVAPIRequestBuilder::resourceURL(path.toStdString());
    QString url = QString::fromStdString(photoURL);

    request->get(url);
}

void SKOSVAPIManager::getMetadata(std::function<void (QByteArray metadata)> success_function,
             const QString &path) {
    HTTPRequest *request = new HTTPRequest(NULL,m_manager);

    request->setHandlerFunc([=] (QNetworkReply *reply) {
        if (reply) {
            QByteArray data = reply->readAll();

            if (success_function) {
                success_function(data);
            }
        }

        //delete captured request
        delete request;
    });

    const std::string metaURL = SKOSVAPIRequestBuilder::resourceURL(path.toStdString());
    QString url = QString::fromStdString(metaURL);

    qDebug() << url;

    request->get(url);
}
