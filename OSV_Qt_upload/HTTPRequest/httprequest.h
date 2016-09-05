#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include "httprequest_global.h"
#include <QObject>
#include <functional>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>

class HTTPREQUESTSHARED_EXPORT HTTPRequest : public QObject
{
    Q_OBJECT
public:
    explicit HTTPRequest(QObject *parent = 0, QNetworkAccessManager *networkManager = 0);

    void post(const QString &url, QHttpMultiPart *multipart = nullptr);
    void post(const QString &url, QMap<QString, QString> &postData);
    void get(const QString &url);

    // added get function
    void getDetails(const QString &url, const QMap<QString, QString>& param);

    void setHandlerFunc (std::function<void (QNetworkReply *reply)> handler_func)
    {
        _handler_func = handler_func;
    }

signals:
    void newBytesDifference(qint64 bytesDiff);
public slots:

private slots:
    void onRequestCompleted();
    void onRequestFailed(QNetworkReply::NetworkError error);
    void onUploadProgress(qint64 sentBytes, qint64 totalBytes);

private:
    std::function<void (QNetworkReply *reply)> _handler_func;

    QNetworkAccessManager *m_manager;
    qint64                m_lastBytesSent;
};


#endif // HTTPREQUEST_H

