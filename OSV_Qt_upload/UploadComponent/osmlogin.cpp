#include "osmlogin.h"
#include <QXmlStreamReader>

const QString CONSUMER_KEY = "M8xSTBR7UeIHHaCYfWeogh8RhQjlhVzi58wLMqWO";
const QString CONSUMER_SECRET_KEY = "1UUeOGTOXfrGhL4eXFTNt2MqWdRYiC8h5IyfSyA6";

OSMLogin::OSMLogin()
 :m_kqoAuthManager(nullptr)
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
    if(m_kqoAuthManager != nullptr)
    {
        delete m_kqoAuthManager;
        m_kqoAuthManager = nullptr;
    }

    m_kqoAuthManager = new KQOAuthManager(this);
    m_kqoAuthManager->setHandleUserAuthorization(true);
    QObject::connect(m_kqoAuthManager, SIGNAL(temporaryTokenReceived(QString,QString)), this, SLOT(onTemporaryTokenReceived(QString,QString)));
    QObject::connect(m_kqoAuthManager, SIGNAL(authorizationReceived(QString,QString)), this, SLOT(onAuthorizationReceived(QString,QString)));
    QObject::connect(m_kqoAuthManager, SIGNAL(accessTokenReceived(QString,QString)), this, SLOT(onAccessTokenReceived(QString,QString)));
    QObject::connect(m_kqoAuthManager, SIGNAL(requestReady(QByteArray)), this, SLOT(onRequestReady(QByteArray)));
}

void OSMLogin::initiateLogin()
{
    resetManager();
    m_kqoAuthRequest->initRequest(KQOAuthRequest::TemporaryCredentials, QUrl("https://www.openstreetmap.org/oauth/request_token"));
    m_kqoAuthRequest->setConsumerKey(CONSUMER_KEY);
    m_kqoAuthRequest->setConsumerSecretKey(CONSUMER_SECRET_KEY);

    m_kqoAuthManager->executeRequest(m_kqoAuthRequest);
    qDebug()<<"EXECUTED REQUEST";
}

void OSMLogin::onAuthorizationReceived(QString token, QString verifier)
{
    qDebug() << "User authorization received: " << token << verifier;

    m_kqoAuthManager->getUserAccessTokens(QUrl("https://www.openstreetmap.org/oauth/access_token"));

    if( m_kqoAuthManager->lastError() != KQOAuthManager::NoError)
    {
        qDebug() << "Error: " << m_kqoAuthManager->lastError();
        m_isLoggedIn = false;
    }
}

void OSMLogin::onAccessTokenReceived(const QString token, const QString tokenSecret)
{
    qDebug() << "Access token received: " << token << tokenSecret;

    m_oauthSettings.setValue("oauth_token", token);
    m_oauthSettings.setValue("oauth_token_secret", tokenSecret);

    qDebug() << "Access tokens now stored.";
    m_isLoggedIn = true;
    m_token = token;
    emit successfulLogin();
}

void OSMLogin::onTemporaryTokenReceived(QString token, QString tokenSecret)
{
    qDebug() << "Temporary token received: " << token << tokenSecret;

    QUrl userAuthURL("https://www.openstreetmap.org/oauth/authorize");

    if( m_kqoAuthManager->lastError() == KQOAuthManager::NoError) {
        qDebug() << "Asking for user's permission to access protected resources. Opening URL: " << userAuthURL;
        m_kqoAuthManager->getUserAuthorization(userAuthURL);
    }
}

void OSMLogin::onAuthorizedRequestDone()
{
    qDebug() << "Request sent";
}

void OSMLogin::onRequestReady(QByteArray response)
{
    qDebug() << "Response from the service: " << response;

    QXmlStreamReader reader(response);

    QString externalUserId;
    QString userName;
    while (!reader.atEnd())
    {
        if(reader.isStartElement())
        {
            qDebug() << "reader name: " << reader.name();
            if (reader.name() == "user")
            {
                QXmlStreamAttributes attributes = reader.attributes();

                externalUserId = attributes.value("id").toString();
                userName = attributes.value("display_name").toString();

            }
        }
        reader.readNext();
    }

    if (userName.isEmpty() || userName == "" || userName.isNull())
    {
        return;
    }
    else
    {
        emit receivedUserInfo();
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

void OSMLogin::getUserDetails()
{
    if (m_oauthSettings.value("oauth_token").toString().isEmpty() ||
        m_oauthSettings.value("oauth_token_secret").toString().isEmpty())
    {
        qDebug() << "No access tokens. Aborting.";
        return;
    }

    m_kqoAuthRequest->initRequest(KQOAuthRequest::AuthorizedRequest, QUrl("https://www.openstreetmap.org/api/0.6/user/details.json"));
    m_kqoAuthRequest->setHttpMethod(KQOAuthRequest::GET);
    m_kqoAuthRequest->setConsumerKey(CONSUMER_KEY);
    m_kqoAuthRequest->setConsumerSecretKey(CONSUMER_SECRET_KEY);
    m_kqoAuthRequest->setToken(m_oauthSettings.value("oauth_token").toString());
    m_kqoAuthRequest->setTokenSecret(m_oauthSettings.value("oauth_token_secret").toString());

    m_kqoAuthManager->executeRequest(m_kqoAuthRequest);
}

void OSMLogin::setIsLoggedIn (bool isLoggedIn)
{
    m_isLoggedIn = isLoggedIn;
}

void OSMLogin::setClientToken(QString& clientToken)
{
    clientToken = m_token;
}
