#include "logincontroller.h"
#include "OSVAPI.h"
#include "QQmlApplicationEngine"
#include "uploadcomponentconstants.h"
#include <QDir>
#include <QHttpPart>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

// HACK - made to put the directories in the same place for 2 different OS
inline QDir getCurrentFolder()
{
    QDir current(QCoreApplication::applicationDirPath());
#ifdef Q_OS_MACX
    current.cdUp();
    current.cdUp();
#endif

    return current;
}

LoginController::LoginController(OSMLogin* osmLogin)
    : m_osmLogin(osmLogin)
    , m_tokenFilePath(getCurrentFolder().path() + "/userDetails.ini")  // path declared as const
                                                                       // member to make sure that
                                                                       // QApplication object is
                                                                       // initialized
{
    QObject::connect(m_osmLogin, SIGNAL(successfulLogin()), this, SLOT(loginSuccess()));
    QObject::connect(m_osmLogin, SIGNAL(failedLogin()), this, SLOT(loginFailedMessage()));

    checkIfLoggedIn();
}

bool LoginController::checkIfUserFileExists()
{
    const QFileInfo checkFile(m_tokenFilePath);
    return (checkFile.exists() && checkFile.isFile());
}

/*
 *  The login() function checks if the user has logged in before (checks if the 'userDetails.ini'
 * file exists)
 *  If the file exists, it uses the user details from the file, else it sends the user to the
 * openstreetmap.org to login and obtain access token
 */
void LoginController::login()
{
    const bool userFileExists = checkIfUserFileExists();
    m_osmLogin->setIsLoggedIn(userFileExists);
    setIsLoggedIn(userFileExists);

    qDebug() << "IS LOGGED IN: " << m_osmLogin->isLoggedIn();
    if (!userFileExists)  // if the user is not logged in (userDetails.ini file does not exist)
    {
        m_osmLogin->initiateLogin();
    }
    else  // the user is logged in (userDetails.ini file exists)
    {
        loadUserInfoFromFile();
    }
}

void LoginController::logout()
{
    if (checkIfUserFileExists())
    {
        QFile::remove(m_tokenFilePath);
        setIsLoggedIn(false);
        m_osmLogin->setIsLoggedIn(false);
    }
}

void LoginController::loginSuccess()
{
    // after the user logs in, a request is sent to obtain user information (username, external user
    // id, client token) used for uploading photos
    m_osmLogin->setTokens(m_requestToken, m_secretToken);
    setIsLoggedIn(true);
    qDebug() << "LOGIN SUCCESSFUL";
    requestAccessTokenFromOSV();
}

void LoginController::loginFailedMessage()
{
    qDebug() << "LOGIN FAILED";
}

/*
 *  This function takes the user details that are stored in the .ini file
 */
void LoginController::loadUserInfoFromFile()
{
    // gets and stores the user details from the .ini file
    const QSettings userDetailsFile(m_tokenFilePath, QSettings::IniFormat);
    m_accessToken = userDetailsFile.value("userToken").toString();

    if (m_accessToken.isEmpty())
    {
        QFile::remove(m_tokenFilePath);
        login();
    }
}

/*
 * stores user information obained from the request in "m_user"
 */
void LoginController::setUserInfo()
{
    if (!m_accessToken.isEmpty())  // if the .ini file exists, it continues with the data obtained
                                   // from the file in order to upload the photos
    {
        QSettings userDetailsFile(m_tokenFilePath, QSettings::IniFormat);
        userDetailsFile.setValue("userToken", m_accessToken);
    }
    else
    {
        QFile::remove(m_tokenFilePath);
        login();
    }
}

void LoginController::checkIfLoggedIn()
{
    const bool userFileExists = checkIfUserFileExists();
    if (userFileExists)
    {
        loadUserInfoFromFile();  // workaround for loading the user info at startup
    }

    setIsLoggedIn(userFileExists);
}

void LoginController::requestAccessTokenFromOSV()
{
    HTTPRequest* request = new HTTPRequest(NULL, new QNetworkAccessManager());

    request->setHandlerFunc([=](QNetworkReply* reply) {
        if (reply)
        {
            QByteArray data        = reply->readAll();
            QString    string_data = QString::fromLatin1(data.data());
            qDebug() << string_data;

            QJsonObject json = OSVAPI::objectFromString(string_data);
            if (!json.isEmpty())
            {
                QJsonObject   statusObj  = json["status"].toObject();
                OSVStatusCode statusCode = (OSVStatusCode)statusObj["apiCode"].toString().toInt();
                if (statusCode == OSVStatusCode::SUCCESS)
                {
                    QJsonObject osvObj = json["osv"].toObject();
                    if (osvObj.contains("access_token"))
                    {
                        qDebug() << osvObj["access_token"].toString();
                        m_accessToken = osvObj["access_token"].toString();
                        setUserInfo();
                    }
                }
            }
        }
        reply->deleteLater();
        delete request;
    });

    if (m_requestToken.isEmpty() || m_secretToken.isEmpty())
    {
        qDebug() << "Missing request token or secret token!";
    }
    else
    {
        QMap<QString, QString> map = QMap<QString, QString>();
        map.insert("request_token", m_requestToken);
        map.insert("secret_token", m_secretToken);

        const QString url(kProtocol + kBaseProductionUrl + "auth/openstreetmap/client_auth");
        qDebug() << "Request URL: " << url;
        request->post(url, map);
    }
}

QString LoginController::getClientToken()
{
    return m_accessToken;
}

void LoginController::setIsLoggedIn(const bool isLoggedIn)
{
    m_isLoggedIn = isLoggedIn;
    emit isLoggedInChanged(isLoggedIn);
}
