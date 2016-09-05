#ifndef SKOSVAPIMANAGER_H
#define SKOSVAPIMANAGER_H

#include <QObject>

#include <vector>
#include <math.h>
#include <QImage>
#include "httprequest.h"
#include <QtMath>
#include "sequence.h"
#include "photo.h"
#include "metadata.h"

static const float KEquatorialRadius = 6378137;

#define radiansToDegrees(_ANGLE_)       (((_ANGLE_) / M_PI) * 180.0f)
#define degreesToRadians(x)             (M_PI * (x) / 180.0)

struct Coordinate {
    float latitude;
    float longitude;
};

struct BoundingBox{
    Coordinate topLeft;
    Coordinate bottomRight;

    void makeBoundingBox(BoundingBox& bound, Coordinate topLeft, Coordinate bottomRight){
        bound.topLeft = topLeft;
        bound.bottomRight = bottomRight;
    }
    void makeBoundingBox(BoundingBox& bound, double lat, double lon, int radius){
        float R = KEquatorialRadius / 1000.0; // earth radius in km

        float factor = cos(degreesToRadians(lat));
        float latFactor = radiansToDegrees(radius / R);
        float longFactor = radiansToDegrees(radius / R / factor);

        Coordinate topLeft;
        topLeft.latitude = lat + latFactor;
        topLeft.longitude = lon - longFactor;

        Coordinate bottomRight;
        bottomRight.latitude = lat - latFactor;
        bottomRight.longitude = lon + longFactor;

        makeBoundingBox(bound, topLeft,bottomRight);
    }
};

class HTTPREQUESTSHARED_EXPORT SKOSVAPIManager : public QObject
{
    Q_OBJECT
public:
    explicit SKOSVAPIManager(QObject *parent = 0);
    ~SKOSVAPIManager();
    void getEcho(std::function<void (const int revisionNo)> success_function, std::function<void ()> failed_function);
    void getList(std::function<void (QList<Sequence*> sequences, int totalPages)> success_function, std::function<void ()> failed_function,
                 const QString &username = NULL,
                 const QString &location = NULL,
                 const QString &startDate = NULL,
                 const QString &endDate = NULL,
                 const int &page = 1,
                 const int &ipp = 10,
                 const int &obdInfo = 0,
                 const QString &platformName = NULL,
                 const BoundingBox &bbox = { {0.0f,0.0f} ,{0.0f,0.0f} } );
    void getPhotoList(std::function<void (QList<Photo*> photos)> success_function, std::function<void ()> failed_function,
                 const QString &sequenceId);
    void getPhoto(std::function<void (QImage image)> success_function,
                 const QString &path);
    void getMetadata(std::function<void (QByteArray metadata)> success_function,
                 const QString &path);
signals:

public slots:

private:
    QNetworkAccessManager *m_manager;

};

#endif // SKOSVAPIMANAGER_H
