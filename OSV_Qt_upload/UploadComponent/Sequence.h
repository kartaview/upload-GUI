#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <qqmlhelpers.h>
#include <QList>
#include <QVariantList>
#include <QJsonArray>
#include <QJsonDocument>
#include "jsonserializable.h"
#include "photo.h"


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
    QML_WRITABLE_PROPERTY (int , photo_no)
    QML_WRITABLE_PROPERTY (double, se_lat)
    QML_WRITABLE_PROPERTY (double, se_lng)
    QML_WRITABLE_PROPERTY (QString, thumb_name)
    QML_WRITABLE_PROPERTY (int, userId)
    QML_WRITABLE_PROPERTY (int, reviewed)
    QML_WRITABLE_PROPERTY (int, changes)
    QML_WRITABLE_PROPERTY (int, recognitions)
    QML_WRITABLE_PROPERTY (QList<QObject*>, photos)
    QML_WRITABLE_PROPERTY (QVariantList, rawLocationData)
    //QML_WRITABLE_PROPERTY (MetadataQTWrapper*, metadata)
    QML_WRITABLE_PROPERTY (int, obd_info)

public:
    explicit Sequence(QObject *parent = 0);

    ImageStatus::Type toImageStatus (const QJsonValue &value);

    using JsonSerializable::write;
    void write(QJsonObject &jsonObj)
    {

    }

    using JsonSerializable::read; //compile
    void read(const QJsonObject &jsonObj)
    {
        if (jsonObj.contains("sequence_id")) {
            this->m_id = jsonObj["sequence_id"].toString().toInt();
        }
        if (jsonObj.contains("id")) {
            this->m_id = jsonObj["id"].toString().toInt();
        }
        //{"address":"Baltimore-Harrisburg Expressway, Sparks, MD, Maryland, United States of America, US",
//        "changes":null,"count_active_photos":"28","date_added":"2016-02-25 16:22:12","obd_info":null,"recognitions":null,"reviewed":null,"sequence_id":"72"


        this->m_address = jsonObj["address"].toString();
        this->m_country_code = jsonObj["country_code"].toString();
        this->m_current_lat = jsonObj["current_lat"].toString().toDouble();
        this->m_current_lng = jsonObj["current_lng"].toString().toDouble();
        this->m_date_added = jsonObj["date_added"].toString();
        this->m_image_processing_status = toImageStatus(jsonObj["image_processing_status"]);

        if (jsonObj.contains("address")) {
            this->m_location = jsonObj["address"].toString();
        }
        if (jsonObj.contains("location")) {
            this->m_location = jsonObj["location"].toString();
        }

        this->m_meta_data_filename = jsonObj["meta_data_filename"].toString();
        this->m_nw_lat = jsonObj["nw_lat"].toString().toDouble();
        this->m_nw_lng = jsonObj["nw_lng"].toString().toDouble();

        if (jsonObj.contains("photo_no")) {
            this->m_photo_no = jsonObj["photo_no"].toString().toInt();
        }
        if (jsonObj.contains("count_active_photos")) {
            this->m_photo_no = jsonObj["count_active_photos"].toString().toInt();
        }

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

//        if(jsonObj.contains("track")) {
//            QJsonArray trackCoordinates = jsonObj["track"].toArray();

//            for (int i = 0; i < trackCoordinates.size(); i+=2) {
//                QJsonValue vLat = trackCoordinates.at(i);
//                QJsonValue vLon = trackCoordinates.at(i+1);

//                QVariantMap variantMap;
//                variantMap.insert("latitude",vLat.toString().toDouble());
//                variantMap.insert("longitude",vLon.toString().toDouble());

//                emptyLocations.append(variantMap);
//            }

//            this->m_rawLocationData = emptyLocations;
//        }
    }


};

#endif // SEQUENCE_H
