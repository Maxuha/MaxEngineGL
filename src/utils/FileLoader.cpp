//
// Created by zykov on 3/22/2026.
//

#include "FileLoader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

std::string FileLoader::LoadFile(const char *path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error(std::string("Failed to open shader file: ") + path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
