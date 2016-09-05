#include "httprequest.h"
#include <QStringList>
#include <QString>
#include <QCoreApplication>
#include <QDebug>
#include <QUrlQuery>
#include <QHttpMultiPart>

HTTPRequest::HTTPRequest(QObject *parent, QNetworkAccessManager *networkManager)
    : QObject(parent),
      m_lastBytesSent(0)
{
    if (!networkManager) {
        m_manager = new QNetworkAccessManager(this);
    }
    else {
        m_manager = networkManager;
    }
}

void HTTPRequest::post(const QString &url, QHttpMultiPart *multipart)
{
    QNetworkRequest request;
    QNetworkReply* reply(nullptr);
    request.setUrl(QUrl(url));

    if (multipart)
    {
        reply = m_manager->post(request, multipart);
        multipart->setParent(reply);
    }
    else
    { // empty Body request
        QByteArray emptyBody;
        reply = m_manager->post(request, emptyBody);
    }

    connect(reply, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(onUploadProgress(qint64,qint64)));
    connect(reply, SIGNAL(finished()), this, SLOT(onRequestCompleted()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onRequestFailed(QNetworkReply::NetworkError)));
}

QHttpPart part_parameter(QString key, QString value) {
    QHttpPart part;
    part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"" + key + "\""));
    part.setBody(value.toLatin1());
    return part;
}

void HTTPRequest::post(const QString &url, QMap<QString, QString> &postData)
{   
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    foreach (QString key, postData.keys()) {
        multiPart->append(part_parameter(key, postData[key]));
    }
    return post(url,multiPart);
}

void HTTPRequest::get(const QString &url)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));

    QNetworkReply *reply = m_manager->get(request);

    connect(reply, SIGNAL(finished()), this, SLOT(onRequestCompleted()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onRequestFailed(QNetworkReply::NetworkError)));
}

void HTTPRequest::onRequestCompleted() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (_handler_func)
    {
        _handler_func(reply);
    }
}

void HTTPRequest::onRequestFailed(QNetworkReply::NetworkError error) {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (_handler_func)
    {
        _handler_func(reply);
    }
}

void HTTPRequest::onUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    if(bytesSent <= 0 || bytesTotal <= 0)
    {
        return;
    }
    else
    {
        qint64 bytesDiff = bytesSent - m_lastBytesSent;
        m_lastBytesSent = bytesSent;
        emit newBytesDifference(bytesDiff);
    }
}
