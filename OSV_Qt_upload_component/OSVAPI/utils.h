#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>

class Utils
{
public:
    Utils();

    static std::vector<std::string> split_string(const std::string& str,
                                          const std::string& delimiter)
    {
        std::vector<std::string> strings;

        std::string::size_type pos = 0;
        std::string::size_type prev = 0;
        while ((pos = str.find(delimiter, prev)) != std::string::npos)
        {
            strings.push_back(str.substr(prev, pos - prev));
            prev = pos + 1;
        }

        // To get the last substring (or only, if delimiter is not found)
        strings.push_back(str.substr(prev));

        return strings;
    }
};

#endif // UTILS_H
