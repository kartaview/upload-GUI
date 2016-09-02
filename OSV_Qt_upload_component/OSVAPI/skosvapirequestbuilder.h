#ifndef SKOSVAPIREQUESTBUILDER_H
#define SKOSVAPIREQUESTBUILDER_H

#include <QObject>
#include <string>
#include "skosvapiconstants.h"

/*
 * Builds url objects for each type of request supported by OSV API
*/
class SKOSVAPIRequestBuilder : public QObject
{
    Q_OBJECT
public:
    explicit SKOSVAPIRequestBuilder(QObject *parent = 0);
    static const std::string echoURL();
    static const std::string listURL();
    static const std::string photoListURL();
    static const std::string resourceURL(std::string path);
private:
    static const std::string baseURL();
};

#endif // SKOSVAPIREQUESTBUILDER_H
