//
// Created by zykov on 4/8/2026.
//

#ifndef MAXENGINE_MESHASSETMANAGER_H
#define MAXENGINE_MESHASSETMANAGER_H
#include <map>
#include <string>
#include "../graphics/Mesh.h"


class ModelImporter;

class MeshAssetManager {
public:
    explicit MeshAssetManager(std::unordered_map<std::string, ModelImporter*> modelImporters) : importers(std::move(modelImporters)) {}

    Mesh* Import(std::string path);

private:
    std::unordered_map<std::string, ModelImporter*> importers;
    std::string GetExtension(const std::string& path);
};



#endif //MAXENGINE_MESHASSETMANAGER_H
