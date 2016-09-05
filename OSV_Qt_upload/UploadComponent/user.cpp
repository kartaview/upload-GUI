#include "user.h"
#include <QSettings>

User::User(QObject *parent)
    : QObject(parent),
      m_externalUserId(-1),
      m_userType("osm"),
      m_username(""),
      m_clientToken("")
{
}

bool User::isUserValid() const
{
    return !get_username().isEmpty() &&
           !get_userType().isEmpty() &&
           !get_clientToken().isEmpty() &&
            get_externalUserId () != -1;
}

void User::loadUserInfoFromFile(const QString& userDetailsFilePath)
{
    const QSettings userDetailsFile (userDetailsFilePath, QSettings::IniFormat);
    set_username (userDetailsFile.value("userName").toString ());
    set_externalUserId (userDetailsFile.value("externalUserId").toInt ());
    set_userType (userDetailsFile.value("userType").toString ());
    set_clientToken (userDetailsFile.value("userToken").toString ());
}

void User::saveUserInfoToFile(const QString& userDetailsFilePath) const
{
    QSettings userDetailsFile (userDetailsFilePath, QSettings::IniFormat);
    // saves user details to .ini file
    userDetailsFile.setValue ("userName", get_username ());
    userDetailsFile.setValue ("externalUserId", get_externalUserId ());
    userDetailsFile.setValue ("userType", get_userType ());
    userDetailsFile.setValue ("userToken", get_clientToken ());
}
