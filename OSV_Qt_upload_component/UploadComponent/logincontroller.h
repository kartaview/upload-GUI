#ifndef LOGINCONTROLLER_H
#define LOGINCONTROLLER_H

#include <osmlogin.h>
#include "user.h"
#include "httprequest.h"

class LoginController : public QObject
{
    Q_OBJECT
    QML_WRITABLE_PROPERTY (bool, isLoggedIn)

public:
    LoginController(OSMLogin *osmLogin);
    User* getUser();

public slots:
    Q_INVOKABLE void login();
    Q_INVOKABLE void logout();

    void loginSuccess();
    void loginFailedMessage();
    void onSetUserInfo();
    void checkIfLoggedIn();

private:
    void loadUserInfoFromFile();
    bool checkIfUserFileExists();

    const QString m_userDetailFilePath;
    OSMLogin *m_osmLogin;
    User* m_user;
};

#endif // LOGINCONTROLLER_H
