#ifndef JSONSERIALIZABLE_H
#define JSONSERIALIZABLE_H

#include <QJsonObject>
#include <QObject>

class JsonSerializable : public QObject
{
     Q_OBJECT
public:
    explicit JsonSerializable(QObject *parent = 0);
    virtual void read(const QJsonObject &jsonObj) = 0;
    virtual void write(QJsonObject &jsonObj) = 0;
};

#endif // JSONSERIALIZABLE_H
