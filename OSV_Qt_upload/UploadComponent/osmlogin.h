#ifndef OSMLOGIN_H
#define OSMLOGIN_H

#include "kqoauthmanager.h"
#include "kqoauthrequest.h"
#include "qsettings.h"
#include <QCoreApplication>
#include <QObject>
#include <QtKOAuth>

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

    void setIsLoggedIn(bool isLoggedIn);
    void setTokens(QString& token, QString& secretToken);

signals:
    void successfulLogin();
    void failedLogin();

private slots:
    void onTemporaryTokenReceived(QString temporaryToken, QString temporaryTokenSecret);
    void onAuthorizationReceived(QString token, QString verifier);
    void onAccessTokenReceived(QString token, QString tokenSecret);

private:
    void resetManager();

private:
    QSettings       m_oauthSettings;
    KQOAuthRequest* m_kqoAuthRequest;
    KQOAuthManager* m_kqoAuthManager;
    bool            m_isLoggedIn = false;
    QString         m_token;
    QString         m_secretToken;
};

#endif  // OSMLOGIN_H
