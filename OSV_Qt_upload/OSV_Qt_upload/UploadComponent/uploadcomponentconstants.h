#ifndef SKOSVAPICONSTANTS_H
#define SKOSVAPICONSTANTS_H

#include <QString>

/*
Protocols
*/
static const QString kProtocol = "http://";

/*
Base url
*/
static const QString kBaseProductionUrl = "openstreetview.com/";

/*
Version
*/
static const QString kVersion = "1.0/";

/*
Command
*/
static const QString kCommandEcho = "echo/";

static const QString kCommandSequence = "sequence/";
static const QString kCommandSequenceFinished = "sequence/finished-uploading/";

static const QString kCommandPhoto = "photo/";
static const QString kCommandVideo = "video/";


/*
Constant numbers
*/
static const int kGigaByte = 1073741824;
static const int kCountThreads = 6;

/*
Status Codes
*/

enum class OSVStatusCode : int {  BAD_LOGIN = 401,
                                  SUCCESS = 600,
                                  SUCCESS_SAVE_ERROR = 602,
                                  ARGUMENT_MISSING = 610,
                                  ARGUMENT_WRONG_TYPE = 611,
                                  ARGUMENT_OUT_RANGE = 612,
                                  NOT_ALLOWED = 618,
                                  DUPLICATE = 660,
                                  STATUS_INCORRECT = 671,
                                  UNEXPECTED_ERROR = 690};
/*
File Status
*/
enum class FileStatus : int {   AVAILABLE = 0,
                                BUSY = 1,
                                DONE = 2};

#endif
