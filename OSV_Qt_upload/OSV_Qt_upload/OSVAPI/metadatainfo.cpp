#include "metadatainfo.h"
#include <string>
#include <stdlib.h>     /* atof */
#include <map>
#include "utils.h"
#include "limits.h"

#define timestamp "timestamp"
#define longitude "longitude"
#define latitude "latitude"
#define elevation "elevation"
#define horizontal_accuracy "horizontal_accuracy"
#define GPSspeed "GPSspeed"
#define yaw "yaw"
#define pitch "pitch"
#define roll "roll"
#define gyroX "gyroX"
#define gyroY "gyroY"
#define gyroZ "gyroZ"
#define accelerationX "accelerationX"
#define accelerationY "accelerationY"
#define accelerationZ "accelerationZ"
#define magneticX "magneticX"
#define magneticY "magneticY"
#define magneticZ "magneticZ"
#define pressure "pressure"
#define compass "compass"
#define index "index"
#define gravityX "gravityX"
#define gravityY "gravityY"
#define gravityZ "gravityZ"
#define OBD2speed "OBD2speed"

std::map<std::string, std::vector<std::string>> version_map = {{kVersionInvalid, {timestamp,longitude,latitude,elevation,horizontal_accuracy,gyroX,gyroY,gyroZ,accelerationX,accelerationY,accelerationZ,pressure,magneticX,magneticY,magneticZ,index}},
                                                    {kVersion101, {timestamp,longitude,latitude,elevation,horizontal_accuracy,gyroX,gyroY,gyroZ,accelerationX,accelerationY,accelerationZ,pressure,compass,index}},
                                                    {kVersion102, {timestamp,longitude,latitude,elevation,horizontal_accuracy,gyroX,gyroY,gyroZ,accelerationX,accelerationY,accelerationZ,pressure,compass,index}},
                                                    {kVersion103, {timestamp,longitude,latitude,elevation,horizontal_accuracy,gyroX,gyroY,gyroZ,accelerationX,accelerationY,accelerationZ,pressure,compass,index,gravityX,gravityY,gravityZ}},
                                                    {kVersion104, {timestamp,longitude,latitude,elevation,horizontal_accuracy,yaw,pitch,roll,accelerationX,accelerationY,accelerationZ,pressure,compass,index,gravityX,gravityY,gravityZ}},
                                                    {kVersion105, {timestamp,longitude,latitude,elevation,horizontal_accuracy,GPSspeed,yaw,pitch,roll,accelerationX,accelerationY,accelerationZ,pressure,compass,index,gravityX,gravityY,gravityZ}},
                                                    {kVersion106, {timestamp,longitude,latitude,elevation,horizontal_accuracy,GPSspeed,yaw,pitch,roll,accelerationX,accelerationY,accelerationZ,pressure,compass,index,gravityX,gravityY,gravityZ,OBD2speed}},
                                                    {kVersion107, {timestamp,longitude,latitude,elevation,horizontal_accuracy,GPSspeed,yaw,pitch,roll,accelerationX,accelerationY,accelerationZ,pressure,compass,index,gravityX,gravityY,gravityZ,OBD2speed}},
                                                    {kVersion108, {timestamp,longitude,latitude,elevation,horizontal_accuracy,GPSspeed,yaw,pitch,roll,accelerationX,accelerationY,accelerationZ,pressure,compass,index,gravityX,gravityY,gravityZ,OBD2speed}}};


MetaDataInfo::MetaDataInfo(const std::string &metaInfo, const std::string &version)
{
    _timestamp = 0;
    _longitude = INT_MIN;
    _latitude = INT_MIN;
    _elevation = INT_MIN;
    _horizontal_accuracy = INT_MIN;
    _GPSspeed = INT_MIN;
    _yaw = INT_MIN;
    _pitch = INT_MIN;
    _roll = INT_MIN;
    _gyroX = INT_MIN;
    _gyroY = INT_MIN;
    _gyroZ = INT_MIN;
    _magneticX = INT_MIN;
    _magneticY = INT_MIN;
    _magneticZ = INT_MIN;
    _accelerationX = INT_MIN;
    _accelerationY = INT_MIN;
    _accelerationZ = INT_MIN;
    _pressure = -1;
    _compass = -1;
    _index = INT_MIN;
    _gravityX = INT_MIN;
    _gravityY = INT_MIN;
    _gravityZ = INT_MIN;
    _OBD2speed = -1;

    std::vector<std::string> subStringList = Utils::split_string(metaInfo,";");
    int j = 0;

    std::vector<std::string> mapping = version_map[version];

    for(auto subString : subStringList) {
        if (j < (int)mapping.size() && subString.size() > 0) {
            std::string key = mapping[j];
            double value = ::atof(subString.c_str());
            setValueForKey(value,key);
        }
        j++;
    }

}

void MetaDataInfo::setValueForKey(const double &value, const std::string &key) {
    if(key.compare(timestamp) == 0) {
        _timestamp = value;
    }
    else if(key.compare(longitude) == 0) {
        _longitude = value;
    }
    else if(key.compare(latitude) == 0) {
        _latitude = value;
    }
    else if(key.compare(elevation) == 0) {
        _elevation = value;
    }
    else if(key.compare(horizontal_accuracy) == 0) {
        _horizontal_accuracy = value;
    }
    else if(key.compare(GPSspeed) == 0) {
        _GPSspeed = value;
    }
    else if(key.compare(yaw) == 0) {
        _yaw = value;
    }
    else if(key.compare(pitch) == 0) {
        _pitch = value;
    }
    else if(key.compare(roll) == 0) {
        _roll = value;
    }
    else if(key.compare(gyroX) == 0) {
        _gyroX = value;
    }
    else if(key.compare(gyroY) == 0) {
        _gyroY = value;
    }
    else if(key.compare(gyroZ) == 0) {
        _gyroZ = value;
    }
    else if(key.compare(accelerationX) == 0) {
        _accelerationX = value;
    }
    else if(key.compare(accelerationY) == 0) {
        _accelerationY = value;
    }
    else if(key.compare(accelerationZ) == 0) {
        _accelerationZ = value;
    }
    else if(key.compare(magneticX) == 0) {
        _magneticX = value;
    }
    else if(key.compare(magneticY) == 0) {
        _magneticY = value;
    }
    else if(key.compare(magneticZ) == 0) {
        _magneticZ = value;
    }
    else if(key.compare(pressure) == 0) {
        _pressure = value;
    }
    else if(key.compare(compass) == 0) {
        _compass = value;
    }
    else if(key.compare(index) == 0) {
        _index = value;
    }
    else if(key.compare(gravityX) == 0) {
        _gravityX = value;
    }
    else if(key.compare(gravityY) == 0) {
        _gravityY = value;
    }
    else if(key.compare(gravityZ) == 0) {
        _gravityZ = value;
    }
    else if(key.compare(OBD2speed) == 0) {
        _OBD2speed = value;
    }
}
