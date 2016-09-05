#include "skosvapirequestbuilder.h"

#pragma mark - Private

const std::string SKOSVAPIRequestBuilder::baseURL() {
    return kProtocol + "://" + kBaseUrl + "/" + kVersion1;
}

#pragma mark - Public

const std::string SKOSVAPIRequestBuilder::echoURL()
{
    const std::string url = baseURL() + "/" + kCommandEcho;
    return url;
}

const std::string SKOSVAPIRequestBuilder::listURL()
{
    const std::string url = baseURL() + "/" + kCommandList;
    return url;
}

const std::string SKOSVAPIRequestBuilder::photoListURL()
{
    const std::string url = baseURL() + "/" + kCommandSequencePhotoList;
    return url;
}

const std::string SKOSVAPIRequestBuilder::resourceURL(std::string path) {
    const std::string url = kProtocol + "://" + kBaseUrl + "/" + path;
    return url;
}
