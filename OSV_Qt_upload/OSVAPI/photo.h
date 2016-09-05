#ifndef PHOTO_H
#define PHOTO_H

/*
 * date_added_f":"2016-02-22  (16:00)",
 * "gps_accuracy":"11.0000",
 * "id":"22",
 * "lat":"46.773279",
 * "lng":"23.590848",
 * "name":"files/photo/2016/2/22/proc/134ce_56cb228f8821d.jpg",
 * "sequence_id":"1",
 * "sequence_index":"22",
 * "th_name":"files/photo/2016/2/22/th/134ce_56cb228f8821d.jpg"})
 *
*/

#include"jsonserializable.h"
#include <qqmlhelpers.h>

class Photo : public JsonSerializable
{
    Q_OBJECT
    QML_WRITABLE_PROPERTY (QString, date_added_f)
    QML_WRITABLE_PROPERTY (double, gps_accuracy)
    QML_WRITABLE_PROPERTY (int, id)
    QML_WRITABLE_PROPERTY (double, lat)
    QML_WRITABLE_PROPERTY (double, lng)
    QML_WRITABLE_PROPERTY (QString, name)
    QML_WRITABLE_PROPERTY (int, sequence_id)
    QML_WRITABLE_PROPERTY (int, sequence_index)
    QML_WRITABLE_PROPERTY (QString, th_name)
    QML_WRITABLE_PROPERTY (QString, lth_name)
public:
    explicit Photo (QObject * parent = NULL) : JsonSerializable (parent) {
        m_date_added_f = "";
        m_gps_accuracy = 0;
        m_id = 0;
        m_lat = 0;
        m_lng = 0;
        m_name = "";
        m_sequence_id = 0;
        m_sequence_index = 0;
        m_th_name = "";
        m_lth_name = "";
    }

    using JsonSerializable::read; //compile
    void read(const QJsonObject &jsonObj)
    {
        this->m_date_added_f = jsonObj["date_added_f"].toString();
        this->m_gps_accuracy = jsonObj["gps_accuracy"].toString().toDouble();
        this->m_id = jsonObj["id"].toString().toInt();
        this->m_lat = jsonObj["lat"].toString().toDouble();
        this->m_lng = jsonObj["lng"].toString().toDouble();
        this->m_name = jsonObj["name"].toString();
        this->m_sequence_id = jsonObj["sequence_id"].toString().toInt();
        this->m_sequence_index = jsonObj["sequence_index"].toString().toInt();
        this->m_th_name = jsonObj["th_name"].toString();
        this->m_lth_name = jsonObj["lth_name"].toString();
    }
};

#endif // PHOTO_H
