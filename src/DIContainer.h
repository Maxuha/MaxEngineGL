//
// Created by zykov on 4/7/2026.
//

#ifndef MAXENGINE_DICONTAINTER_H
#define MAXENGINE_DICONTAINTER_H
#include "IO/AssetManager.h"
#include "IO/ObjImporter.h"
#include "IO/TextureImporter.h"


class DIContainer {
public:
    DIContainer() {
        Init();
    }

    ~DIContainer() {
        delete assetManager;
        delete meshAssetManager;
        delete shaderImporter;
        delete textureImporter;
        delete objImporter;
    }

    void Init();

    AssetManager* assetManager = nullptr;
    MeshAssetManager* meshAssetManager = nullptr;
    ObjImporter* objImporter = nullptr;
    TextureImporter* textureImporter = nullptr;
    ShaderImporter* shaderImporter = nullptr;

};

inline void DIContainer::Init() {
    objImporter = new ObjImporter();
    textureImporter = new TextureImporter();
    shaderImporter = new ShaderImporter();

    meshAssetManager = new MeshAssetManager{
            {
                {
                    ".obj", objImporter
                }
            }
    };

    assetManager = new AssetManager(meshAssetManager, textureImporter, shaderImporter);
}


#endif //MAXENGINE_DICONTAINTER_H
