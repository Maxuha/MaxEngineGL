//
// Created by zykov on 3/22/2026.
//

#ifndef MAXENGINE_FILELOADER_H
#define MAXENGINE_FILELOADER_H
#include <string>


class FileReader {
public:
    static std::string LoadFile(const char *path);
};


#endif //MAXENGINE_FILELOADER_H