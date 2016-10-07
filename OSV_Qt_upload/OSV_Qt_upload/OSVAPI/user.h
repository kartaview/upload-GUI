#ifndef USER_H
#define USER_H

/*
User: {
  id(Number; Int(10))
  type(String|Null; Enum('osm'|'anonymous'))
  externalUserId(Number|Null; Int(10))
  username(String|Null; String(100))
}
*/

#include <qqmlhelpers.h>
#include"jsonserializable.h"

class User : public JsonSerializable
{
    Q_OBJECT
    QML_WRITABLE_PROPERTY (int, id)
    QML_WRITABLE_PROPERTY (QString, type)
    QML_WRITABLE_PROPERTY (int, externalUserId)
    QML_WRITABLE_PROPERTY (QString, username)

public:
    explicit User(QObject *parent = 0);

    using JsonSerializable::read; //compile
    void read(const QJsonObject &jsonObj)
    {
        this->m_id = jsonObj["id"].toString().toInt();
        this->m_type = jsonObj["type"].toString();
        this->m_externalUserId = jsonObj["externalUserId"].toString().toInt();
        this->m_username = jsonObj["username"].toString();
    }
};

#endif // USER_H
