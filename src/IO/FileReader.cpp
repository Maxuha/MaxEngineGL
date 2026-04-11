//
// Created by zykov on 3/27/2026.
//

#include "FileReader.h"
#include <fstream>
#include <sstream>

std::string FileReader::ReadFileString(const char *path) {
    const std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
