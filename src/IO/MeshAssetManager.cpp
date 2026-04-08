//
// Created by zykov on 4/8/2026.
//

#include "MeshAssetManager.h"
#include "ModelImporter.h"

Mesh * MeshAssetManager::Import(std::string path) {
    return importers[GetExtension(path)]->Import(path.c_str());
}

std::string MeshAssetManager::GetExtension(const std::string& path) {
    const size_t dotPos = path.find_last_of('.');
    return (dotPos == std::string::npos) ? path : path.substr(dotPos);
}