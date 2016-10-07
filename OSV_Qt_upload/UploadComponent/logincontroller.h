#ifndef LOGINCONTROLLER_H
#define LOGINCONTROLLER_H

#include "httprequest.h"
#include "qqmlhelpers.h"
#include <osmlogin.h>

class LoginController : public QObject
{
    Q_OBJECT
    QML_READONLY_PROPERTY(bool, isLoggedIn)

public:
    LoginController(OSMLogin* osmLogin);

public slots:
    Q_INVOKABLE void login();
    Q_INVOKABLE void logout();

    void loginSuccess();
    void loginFailedMessage();
    void checkIfLoggedIn();

    QString getClientToken();
    void setIsLoggedIn(const bool isLoggedIn);

private:
    void loadUserInfoFromFile();
    void setUserInfo();
    bool checkIfUserFileExists();
    void requestAccessTokenFromOSV();

    const QString m_tokenFilePath;
    OSMLogin*     m_osmLogin;
    QString       m_requestToken;
    QString       m_accessToken;
    QString       m_secretToken;
};

#endif  // LOGINCONTROLLER_H
