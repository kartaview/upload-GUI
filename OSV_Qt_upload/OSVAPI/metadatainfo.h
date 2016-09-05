#ifndef METADATAINFO_H
#define METADATAINFO_H
#include <vector>

#define kVersionInvalid "ios_first_version"
#define kVersion101 "1.0.1"
#define kVersion102 "1.0.2"
#define kVersion103 "1.0.3"
#define kVersion104 "1.0.4"
#define kVersion105 "1.0.5"
#define kVersion106 "1.0.6"
#define kVersion107 "1.0.7"
#define kVersion108 "1.0.8"

class MetaDataInfo
{
public:
    MetaDataInfo(const std::string &metaInfo, const std::string &version);

    double _timestamp;
    double _longitude;
    double _latitude;
    double _elevation;
    double _horizontal_accuracy;
    double _GPSspeed;
    double _yaw;
    double _pitch;
    double _roll;
    double _gyroX;
    double _gyroY;
    double _gyroZ;
    double _magneticX;
    double _magneticY;
    double _magneticZ;
    double _accelerationX;
    double _accelerationY;
    double _accelerationZ;
    double _pressure;
    double _compass;
    double _index;
    double _gravityX;
    double _gravityY;
    double _gravityZ;
    double _OBD2speed;
    bool   _isInterpolated;
private:
    void setValueForKey(const double &value, const std::string &key);
};

#endif // METADATAINFO_H
