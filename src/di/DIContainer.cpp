#include "DIContainer.h"
#include "../IO/ModelImporter.h"

DIContainer& DIContainer::GetInstance() {
    static DIContainer instance;
    return instance;
}

DIContainer::DIContainer()
{
    objImporter     = std::make_unique<ObjImporter>();
    assimpImporter     = std::make_unique<AssimpImporter>();
    textureImporter = std::make_unique<TextureImporter>();
    shaderImporter  = std::make_unique<ShaderImporter>();

    meshAssetManager = std::make_unique<MeshAssetManager>(
        std::unordered_map<std::string, ModelImporter*>{
            { ".obj", objImporter.get() },
            { ".fbx", assimpImporter.get() }
        });

    assetManager = std::make_unique<AssetManager>(
        meshAssetManager.get(),
        textureImporter.get(),
        shaderImporter.get()
    );

    physicsEngine = std::make_unique<PhysicsEngine>();
    
    Register(physicsEngine.get());
    Register(assetManager.get());
    Register(meshAssetManager.get());
    Register(objImporter.get());
    Register(textureImporter.get());
    Register(shaderImporter.get());
}
