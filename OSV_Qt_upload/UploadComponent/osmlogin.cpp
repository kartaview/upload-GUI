#include "osmlogin.h"
#include <QXmlStreamReader>
const QString CONSUMER_KEY        = "rBWV8Eaottv44tXfdLofdNvVemHOL62Lsutpb9tw";
const QString CONSUMER_SECRET_KEY = "rpmeZIp49sEjjcz91X9dsY0vD1PpEduixuPy8T6S";

OSMLogin::OSMLogin()
    : m_kqoAuthManager(nullptr)
{
    m_kqoAuthRequest = new KQOAuthRequest;
    m_kqoAuthRequest->setEnableDebugOutput(true);
}

OSMLogin::~OSMLogin()
{
    delete m_kqoAuthManager;
    delete m_kqoAuthRequest;
}

void OSMLogin::resetManager()
{
    /* Workaround:
     * KQOAuthManager doesn't behave as expected.
     * It has to be reset in order to properly execute more than one KQOAuthRequest
     * Relative scenario: Login + Logout + Login in the same session
     */
    if (m_kqoAuthManager != nullptr)
    {
        delete m_kqoAuthManager;
        m_kqoAuthManager = nullptr;
    }

    m_kqoAuthManager = new KQOAuthManager(this);
    m_kqoAuthManager->setHandleUserAuthorization(true);
    QObject::connect(m_kqoAuthManager, SIGNAL(temporaryTokenReceived(QString, QString)), this,
                     SLOT(onTemporaryTokenReceived(QString, QString)));
    QObject::connect(m_kqoAuthManager, SIGNAL(authorizationReceived(QString, QString)), this,
                     SLOT(onAuthorizationReceived(QString, QString)));
    QObject::connect(m_kqoAuthManager, SIGNAL(accessTokenReceived(QString, QString)), this,
                     SLOT(onAccessTokenReceived(QString, QString)));
}

void OSMLogin::initiateLogin()
{
    resetManager();
    m_kqoAuthRequest->initRequest(KQOAuthRequest::TemporaryCredentials,
                                  QUrl("https://www.openstreetmap.org/oauth/request_token"));
    m_kqoAuthRequest->setConsumerKey(CONSUMER_KEY);
    m_kqoAuthRequest->setConsumerSecretKey(CONSUMER_SECRET_KEY);

    m_kqoAuthManager->executeRequest(m_kqoAuthRequest);
    qDebug() << "EXECUTED REQUEST";
}

void OSMLogin::onAuthorizationReceived(QString token, QString verifier)
{
    qDebug() << "User authorization received: " << token << verifier;

    m_kqoAuthManager->getUserAccessTokens(QUrl("https://www.openstreetmap.org/oauth/access_token"));

    if (m_kqoAuthManager->lastError() != KQOAuthManager::NoError)
    {
        qDebug() << "Error: " << m_kqoAuthManager->lastError();
        m_isLoggedIn = false;
    }
}

void OSMLogin::onAccessTokenReceived(const QString token, const QString tokenSecret)
{
    qDebug() << "Access token received: " << token << tokenSecret;
    m_token       = token;
    m_secretToken = tokenSecret;
    m_oauthSettings.setValue("oauth_token", token);
    m_oauthSettings.setValue("oauth_token_secret", tokenSecret);

    qDebug() << "Access tokens now stored.";
    m_isLoggedIn = true;

    emit successfulLogin();
}

void OSMLogin::onTemporaryTokenReceived(QString token, QString tokenSecret)
{
    qDebug() << "Temporary token received: " << token << tokenSecret;

    QUrl userAuthURL("https://www.openstreetmap.org/oauth/authorize");

    if (m_kqoAuthManager->lastError() == KQOAuthManager::NoError)
    {
        qDebug() << "Asking for user's permission to access protected resources. Opening URL: "
                 << userAuthURL;
        m_kqoAuthManager->getUserAuthorization(userAuthURL);
    }
}

bool OSMLogin::isLoggedIn()
{
    return m_isLoggedIn;
}

QString OSMLogin::getTokenString()
{
    return m_token;
}

void OSMLogin::setIsLoggedIn(bool isLoggedIn)
{
    m_isLoggedIn = isLoggedIn;
}

void OSMLogin::setTokens(QString& token, QString& secretToken)
{
    token       = m_token;
    secretToken = m_secretToken;
}
