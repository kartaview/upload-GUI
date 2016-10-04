#ifndef LOGINCONTROLLER_H
#define LOGINCONTROLLER_H

#include <osmlogin.h>
#include "qqmlhelpers.h"
#include "httprequest.h"

class LoginController : public QObject
{
    Q_OBJECT
    QML_WRITABLE_PROPERTY (bool, isLoggedIn)

public:
    LoginController(OSMLogin *osmLogin);

public slots:
    Q_INVOKABLE void login();
    Q_INVOKABLE void logout();

    void loginSuccess();
    void loginFailedMessage();
    void onSetUserInfo();
    void checkIfLoggedIn();

    QString getClientToken();

private:
    void loadUserInfoFromFile();
    bool checkIfUserFileExists();

    const QString m_tokenFilePath;
    OSMLogin* m_osmLogin;
    QString   m_clientToken;
};

#endif // LOGINCONTROLLER_H
