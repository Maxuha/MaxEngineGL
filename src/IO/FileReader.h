//
// Created by zykov on 3/27/2026.
//

#ifndef MAXENGINE_FILELOADER_H
#define MAXENGINE_FILELOADER_H
#include <string>

class FileReader {
public:
    static std::string ReadFileString(const char* path);
};

#endif //MAXENGINE_FILELOADER_H
