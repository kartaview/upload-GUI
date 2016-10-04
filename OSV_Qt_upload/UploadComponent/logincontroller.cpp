#include "logincontroller.h"
#include "QQmlApplicationEngine"
#include <QDir>

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

LoginController::LoginController(OSMLogin *osmLogin):
    m_osmLogin(osmLogin),
    m_tokenFilePath(getCurrentFolder().path() + "/userDetails.ini") //path declared as const member to make sure that QApplication object is initialized
{
    QObject::connect(m_osmLogin, SIGNAL(successfulLogin()), this, SLOT(loginSuccess()));
    QObject::connect(m_osmLogin, SIGNAL(failedLogin()), this, SLOT(loginFailedMessage()));
    QObject::connect(m_osmLogin, SIGNAL(receivedUserInfo()), this, SLOT(onSetUserInfo()));

    checkIfLoggedIn();
}

bool LoginController::checkIfUserFileExists()
{
    const QFileInfo checkFile(m_tokenFilePath);
    return (checkFile.exists () && checkFile.isFile ());
}

/*
 *  The login() function checks if the user has logged in before (checks if the 'userDetails.ini' file exists)
 *  If the file exists, it uses the user details from the file, else it sends the user to the openstreetmap.org to login and obtain access token
 */
void LoginController::login()
{
    const bool userFileExists = checkIfUserFileExists ();
    m_osmLogin->setIsLoggedIn (userFileExists);
    set_isLoggedIn(userFileExists);

    qDebug () << "IS LOGGED IN: " << m_osmLogin->isLoggedIn();
    if (!userFileExists) // if the user is not logged in (userDetails.ini file does not exist)
    {
        m_osmLogin->initiateLogin();
    }
    else // the user is logged in (userDetails.ini file exists)
    {
        loadUserInfoFromFile();
    }
}

void LoginController::logout()
{
    if (checkIfUserFileExists())
    {
        QFile::remove(m_tokenFilePath);
        set_isLoggedIn(false);
        m_osmLogin->setIsLoggedIn(false);
    }
}

void LoginController::loginSuccess()
{
    // after the user logs in, a request is sent to obtain user information (username, external user id, client token) used for uploading photos
    m_osmLogin->getUserDetails();
    set_isLoggedIn(true);
    qDebug() << "LOGIN SUCCESSFUL";
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
    const QSettings userDetailsFile (m_tokenFilePath, QSettings::IniFormat);
    m_clientToken = userDetailsFile.value("userToken").toString ();

    if (m_clientToken.isEmpty())
    {
        QFile::remove(m_tokenFilePath);
        login();
    }
}

/*
 * stores user information obained from the request in "m_user"
 */
void LoginController::onSetUserInfo()
{
    m_osmLogin->setClientToken(m_clientToken);
    qDebug() << "CLIENT TOKEN" << m_clientToken;

    if (!m_clientToken.isEmpty()) // if the .ini file exists, it continues with the data obtained from the file in order to upload the photos
    {
        QSettings userDetailsFile (m_tokenFilePath, QSettings::IniFormat);
        userDetailsFile.setValue ("userToken", m_clientToken);
    }
    else
    {
        QFile::remove(m_tokenFilePath);
        login ();
    }
}

void LoginController::checkIfLoggedIn()
{
    const bool userFileExists = checkIfUserFileExists();
    if (userFileExists)
    {
        loadUserInfoFromFile(); // workaround for loading the user info at startup
    }

    set_isLoggedIn(userFileExists);
}

QString LoginController::getClientToken()
{
    return m_clientToken;
}
