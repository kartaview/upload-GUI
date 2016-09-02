#ifndef SEQUENCE_H
#define SEQUENCE_H

/*
 * {
 * "id":"583",
 * "address":"Strada Cuza Vodă, Someșeni, Cluj-Napoca, Romania, RO",
 * "count_active_photos":"149",
 * "country_code":"RO",
 * "current_lat":"46.773419",
 * "current_lng":"23.592858",
 * "date_added":"2016-01-20  (14:25)",
 * "image_processing_status":"PROCESSING_FINISHED",
 * "location":"Strada Cuza Vodă, Someșeni, Cluj-Napoca, Romania, RO",
 * "meta_data_filename":null,
 * "nw_lat":"46.773434",
 * "nw_lng":"23.592749",
 * "photo_no":"149",
 * "se_lat":"46.773290",
 * "se_lng":"23.592860",
 * "thumb_name":"files/photo/2016/1/20/th/583_16ada_56ec164dd2c7d.jpeg",
 * "user_id":"3"})
*/

#include <qqmlhelpers.h>
#include "jsonserializable.h"
#include "photo.h"
#include <QList>
#include <QVariantList>
#include "metadataqtwrapper.h"

QML_ENUM_CLASS (ImageStatus,
                NEW = 0,
                UPLOAD_FINISHED,
                PROCESSING_FINISHED,
                PROCESSING_FAILED)

class Sequence : public JsonSerializable
{
    Q_OBJECT
    QML_WRITABLE_PROPERTY (int, id)
    QML_WRITABLE_PROPERTY (QString, address)
    QML_WRITABLE_PROPERTY (QString, country_code)
    QML_WRITABLE_PROPERTY (double, current_lat)
    QML_WRITABLE_PROPERTY (double, current_lng)
    QML_WRITABLE_PROPERTY (QString, date_added)
    QML_WRITABLE_PROPERTY (ImageStatus::Type, image_processing_status)
    QML_WRITABLE_PROPERTY (QString, location)
    QML_WRITABLE_PROPERTY (QString, meta_data_filename)
    QML_WRITABLE_PROPERTY (double, nw_lat)
    QML_WRITABLE_PROPERTY (double, nw_lng)
    QML_WRITABLE_PROPERTY (int, photo_no)
    QML_WRITABLE_PROPERTY (double, se_lat)
    QML_WRITABLE_PROPERTY (double, se_lng)
    QML_WRITABLE_PROPERTY (QString, thumb_name)
    QML_WRITABLE_PROPERTY (int, userId)
    QML_WRITABLE_PROPERTY (int, reviewed)
    QML_WRITABLE_PROPERTY (int, changes)
    QML_WRITABLE_PROPERTY (int, recognitions)
    QML_WRITABLE_PROPERTY (QList<QObject*>, photos)
    QML_WRITABLE_PROPERTY (QVariantList, rawLocationData)
    QML_WRITABLE_PROPERTY (MetadataQTWrapper*, metadata)
    QML_WRITABLE_PROPERTY (int, obd_info)


public:
    explicit Sequence(QObject *parent = 0);

    ImageStatus::Type toImageStatus (const QJsonValue &value);
    using JsonSerializable::read; //compile

    void read(const QJsonObject &jsonObj)
    {
        this->m_id = jsonObj["id"].toString().toInt();
        this->m_address = jsonObj["address"].toString();
        this->m_country_code = jsonObj["country_code"].toString();
        this->m_current_lat = jsonObj["current_lat"].toString().toDouble();
        this->m_current_lng = jsonObj["current_lng"].toString().toDouble();
        this->m_date_added = jsonObj["date_added"].toString();
        this->m_image_processing_status = toImageStatus(jsonObj["image_processing_status"]);
        this->m_location = jsonObj["location"].toString();
        this->m_meta_data_filename = jsonObj["meta_data_filename"].toString();
        this->m_nw_lat = jsonObj["nw_lat"].toString().toDouble();
        this->m_nw_lng = jsonObj["nw_lng"].toString().toDouble();
        this->m_photo_no = jsonObj["photo_no"].toString().toInt();
        this->m_se_lat = jsonObj["se_lat"].toString().toDouble();
        this->m_se_lng = jsonObj["se_lng"].toString().toDouble();
        this->m_thumb_name = jsonObj["thumb_name"].toString();
        this->m_userId = jsonObj["userId"].toString().toInt();
        this->m_reviewed = jsonObj["reviewed"].toString().toInt();
        this->m_changes = jsonObj["changes"].toString().toInt();
        this->m_recognitions = jsonObj["recognitions"].toString().toInt();
        this->m_obd_info = jsonObj["obd_info"].toString().toInt();

        QVariantList emptyLocations;
        this->m_rawLocationData = emptyLocations;
    }
};

#endif // SEQUENCE_H
