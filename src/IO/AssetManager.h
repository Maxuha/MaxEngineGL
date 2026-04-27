//
// Created by zykov on 4/7/2026.
//

#ifndef MAXENGINE_ASSETMANAGER_H
#define MAXENGINE_ASSETMANAGER_H

// #include <memory>
#include <string>
#include <utility>
#include "MeshAssetManager.h"
#include "ModelImporter.h"
#include "ShaderImporter.h"
#include "TextureImporter.h"


class AssetManager {
public:
    explicit AssetManager(MeshAssetManager *meshAssetManager,
                          TextureImporter *textureImporter,
                          ShaderImporter *shaderImporter) : meshAssetManager(meshAssetManager),
                                                            textureImporter(textureImporter),
                                                            shaderImporter(shaderImporter) {
    }

    template<typename T>
    T* Import(const std::string &path);

private:
    MeshAssetManager *meshAssetManager;
    TextureImporter *textureImporter;
    ShaderImporter *shaderImporter;
};

template<typename T>
T* AssetManager::Import(const std::string &path) {
    if constexpr (std::is_same_v<T, Model>) {
        return meshAssetManager->Import(path);
    } else if constexpr (std::is_same_v<T, Texture>) {
        return textureImporter->Import(path);
    } else if constexpr (std::is_same_v<T, Shader>) {
        return shaderImporter->Import(path);
    }
    return nullptr;
}


#endif //MAXENGINE_ASSETMANAGER_H
