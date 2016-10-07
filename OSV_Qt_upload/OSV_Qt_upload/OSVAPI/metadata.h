#ifndef METADATA_H
#define METADATA_H

#include <vector>
#include <string>
#include "metadatainfo.h"

class Metadata
{
public:
    Metadata();

    static void parseMetaData(const std::string &rawMetadata, Metadata& metadata);

    std::string meta_version;
    std::vector<MetaDataInfo> metadataInfo;
};

#endif // METADATA_H
