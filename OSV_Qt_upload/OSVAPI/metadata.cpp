#include "metadata.h"
#include "utils.h"
#include <QDebug>

Metadata::Metadata()
{

}

void Metadata::parseMetaData(const std::string &rawMetadata, Metadata &metadata)
{

    std::vector<MetaDataInfo>& returnList = metadata.metadataInfo;
    std::vector<std::string> newLinesList = Utils::split_string(rawMetadata,"\n");

    std::string version = kVersionInvalid;

    int i = 0;
    for(auto str : newLinesList) {
        if (i == 0) { //meta info
            std::vector<std::string> splitValues = Utils::split_string(str,";");
            if(splitValues.size() >= 2) {
                //we have version
                version = splitValues.at(2);
            }
        }
        else {
            //meta
            MetaDataInfo metaDataObj(str,version);
            returnList.push_back(metaDataObj);
        }
        i++;
    }

    metadata.meta_version = version;
}
