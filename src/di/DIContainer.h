//
// Created by zykov on 4/7/2026.
//

#ifndef MAXENGINE_DICONTAINER_H
#define MAXENGINE_DICONTAINER_H

#include <any>
#include <memory>
#include <typeindex>

#include "../IWindow.h"
#include "../physics/PhysicsEngine.h"
#include "../IO/AssetManager.h"
#include "../IO/MeshAssetManager.h"
#include "../IO/ObjImporter.h"
#include "../IO/TextureImporter.h"
#include "../IO/shader/ShaderImporter.h"
// #include "../IO/AssimpImporter.h"
#include "../renderer/Renderer.h"
#include "../renderer/IRenderer.h"

class AssimpImporter;


class DIContainer
{
public:

    static DIContainer& GetInstance();

    DIContainer(const DIContainer&) = delete;
    DIContainer& operator=(const DIContainer&) = delete;
    DIContainer(DIContainer&&) = delete;
    DIContainer& operator=(DIContainer&&) = delete;

    template<typename T>
    void Register(T* t) {
        services[typeid(T)] = std::move(t);
    };

    template<typename T>
    T* Get() {
        const auto typeId = std::type_index(typeid(T));
        const auto service = services.find(typeId);
        if (service == services.end()) return nullptr;
        return std::any_cast<T*>(service->second);
    };

private:
    DIContainer();
    ~DIContainer() = default;

    std::unordered_map<std::type_index, std::any> services;

    std::unique_ptr<ObjImporter>                 objImporter;
    std::unique_ptr<AssimpImporter>              assimpImporter;
    std::unique_ptr<TextureImporter>             textureImporter;
    std::unique_ptr<ShaderImporter>              shaderImporter;
    std::unique_ptr<MeshAssetManager>            meshAssetManager;
    std::unique_ptr<AssetManager>                assetManager;
    std::unique_ptr<PhysicsEngine>               physicsEngine;
    std::unique_ptr<Rendering::IRenderer>        renderer;
    std::unique_ptr<IWindow>                     window;
};


#endif // MAXENGINE_DICONTAINER_H


