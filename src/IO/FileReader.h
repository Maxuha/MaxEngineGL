//
// Created by zykov on 3/27/2026.
//

#ifndef MAXENGINE_FILELOADER_H
#define MAXENGINE_FILELOADER_H
#include <string>

class FileReader {
public:
    static std::string ReadFileString(const char* path);
    static std::vector<char> ReadFileBytes(const std::string& path);
};

#endif //MAXENGINE_FILELOADER_H
