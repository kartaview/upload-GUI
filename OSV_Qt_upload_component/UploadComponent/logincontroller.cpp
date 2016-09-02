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
    m_user(new User()),
    m_osmLogin(osmLogin),
    m_userDetailFilePath(getCurrentFolder().path() + "/userDetails.ini") //path declared as const member to make sure that QApplication object is initialized
{
    QObject::connect(m_osmLogin, SIGNAL(successfulLogin()), this, SLOT(loginSuccess()));
    QObject::connect(m_osmLogin, SIGNAL(failedLogin()), this, SLOT(loginFailedMessage()));
    QObject::connect(m_osmLogin, SIGNAL(receivedUserInfo()), this, SLOT(onSetUserInfo()));

    checkIfLoggedIn();
}

bool LoginController::checkIfUserFileExists()
{
    const QFileInfo checkFile(m_userDetailFilePath);
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
        QFile::remove(m_userDetailFilePath);
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
    m_user->loadUserInfoFromFile(m_userDetailFilePath);
    if (!m_user->isUserValid())
    {
        QFile::remove(m_userDetailFilePath);
        login();
    }
}

/*
 * stores user information obained from the request in "m_user"
 */
void LoginController::onSetUserInfo()
{
    m_osmLogin->setUserInfo(m_user);
    qDebug() << "CLIENT TOKEN" << m_user->get_clientToken() << "USER NAME :" << m_user->get_username() << "USER EXTERNAL ID: " <<  m_user->get_externalUserId();

    if (m_user->isUserValid()) // if the .ini file exists, it continues with the data obtained from the file in order to upload the photos
    {
        m_user->saveUserInfoToFile(m_userDetailFilePath);
    }
    else
    {
        QFile::remove(m_userDetailFilePath);
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
User* LoginController::getUser()
{
    return m_user;
}
