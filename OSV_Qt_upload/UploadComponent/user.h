#ifndef USER_H
#define USER_H

#include <qqmlhelpers.h>

class User : public QObject
{
    Q_OBJECT
    QML_WRITABLE_PROPERTY (int, externalUserId)
    QML_WRITABLE_PROPERTY (QString, userType)
    QML_WRITABLE_PROPERTY (QString, username)
    QML_WRITABLE_PROPERTY (QString, clientToken)
public:
    explicit User(QObject *parent = 0);
    bool isUserValid() const;
    void loadUserInfoFromFile(const QString &userDetailsFilePath);
    void saveUserInfoToFile(const QString& userDetailsFilePath) const;
private:

};

#endif // USER_H
