#ifndef OSMLOGIN_H
#define OSMLOGIN_H

#include <QObject>
#include "kqoauthrequest.h"
#include "kqoauthmanager.h"
#include "qsettings.h"
#include <QtKOAuth>
#include <QCoreApplication>
#include "user.h"

class OSMLogin : public QObject
{
    Q_OBJECT
public:
    OSMLogin();
    ~OSMLogin();

    void initiateLogin();
    void xauth();
    bool isLoggedIn();
    QString getTokenString();
    void getUserDetails();
    void setUserInfo(User *user);
    void setIsLoggedIn(bool isLoggedIn);

signals:
    void successfulLogin();
    void failedLogin();
    void receivedUserInfo();

private slots:
    void onTemporaryTokenReceived(QString temporaryToken, QString temporaryTokenSecret);
    void onAuthorizationReceived(QString token, QString verifier);
    void onAccessTokenReceived(QString token, QString tokenSecret);
    void onAuthorizedRequestDone();
    void onRequestReady(QByteArray);

private:
    void resetManager();

private:
    QSettings m_oauthSettings;
    KQOAuthRequest *m_kqoAuthRequest;
    KQOAuthManager *m_kqoAuthManager;
    bool m_isLoggedIn = false;
    QString m_token;
    QString m_userName;
    QString m_externalUserId;

};

#endif // OSMLOGIN_H
