#ifndef SKOSVAPICONSTANTS_H
#define SKOSVAPICONSTANTS_H

//#define HTTPS

#include <string>

//#define ENV_OSV_TEST
//#define ENV_OSV_STAGING 1
#define ENV_OSV_PROD 1

/*
http://[env.]open-street-view.skobbler.net/[version]/[command]/
[command] = please see method names
[version] = 1.0
[env] = tst | null
*/

/*
Protocols
*/
#ifndef HTTPS
static const std::string kProtocol = "http";
#else
static const std::string kProtocol = "https";
#endif

/*
Base url
*/
#ifdef ENV_OSV_TEST
static const std::string kBaseUrl = "tst.open-street-view.skobbler.net";
#elif ENV_OSV_STAGING
static const std::string kBaseUrl = "st-tst.open-street-view.skobbler.net";
#elif ENV_OSV_PROD
static const std::string kBaseUrl = "openstreetview.com";
#endif

/*
Version
*/
static const std::string kVersion1 = "1.0";

/*
Command
*/
static const std::string kCommandEcho = "echo/";

static const std::string kCommandSequence = "sequence/";
static const std::string kCommandSequenceFinished = "sequence/finished-uploading/";
static const std::string kCommandSequencePhotoList = "sequence/photo-list/";
static const std::string kCommandSequenceRemove = "sequence/remove/";

static const std::string kCommandPhoto = "photo/";
static const std::string kCommandPhotoRemove = "remove/";

static const std::string kCommandTracks = "tracks/";
static const std::string kCommandList = "list/";
static const std::string kCommandMyList = "list/my-list/";

/*
Parameters
*/
static const std::string kPage = "page";
static const std::string kIpp = "ipp";
static const std::string kStartDate = "startDate";
static const std::string kEndDate = "endDate";
static const std::string kPhotoId = "photoId";
static const std::string kSequenceId = "sequenceId";
static const std::string kSequenceIndex = "sequenceIndex";

/*
Status Codes
*/

enum class OSVStatusCode : int {  SUCCESS = 600,
                                  SUCCESS_SAVE_ERROR = 602,
                                  ARGUMENT_MISSING = 610,
                                  ARGUMENT_WRONG_TYPE = 611,
                                  ARGUMENT_OUT_RANGE = 612,
                                  NOT_ALLOWED = 618,
                                  STATUS_INCORRECT = 671,
                                  UNEXPECTED_ERROR = 690};


#endif // SKOSVAPICONSTANTS_H
