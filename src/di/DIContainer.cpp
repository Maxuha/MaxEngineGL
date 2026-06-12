#include "DIContainer.h"

#include "../Window.h"
#include "../IO/ModelImporter.h"
#include "../renderer/gl/GLRenderDevice.h"
#include "../renderer/Renderer.h"
#include "../IO/AssimpImporter.h"
#include "../renderer/RendererContext.h"

using namespace Rendering;

DIContainer& DIContainer::GetInstance() {
    static DIContainer instance;
    return instance;
}

DIContainer::DIContainer()
{
    objImporter     = std::make_unique<ObjImporter>();
    assimpImporter  = std::make_unique<AssimpImporter>();
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

    window = std::make_unique<Window>(2560, 1440);
    window->Open();

    Shader* standardLitShader = shaderImporter->Import("phong/phong");
    standardLitShader->AddProperty(ShaderProperty::SHININESS, ShaderMetaProperty(0, sizeof(float)));
    standardLitShader->AddProperty(ShaderProperty::COLOR_DIFFUSE, ShaderMetaProperty(16, sizeof(Vector4)));

    Shader* standardShadowShader = shaderImporter->Import("depth/depth");

    RenderConfig renderConfig;
    renderConfig.api = RenderAPI::OpenGL;
    renderConfig.litShader = standardLitShader;
    renderConfig.shadowShader = standardShadowShader;

    if (auto* contextPtr = dynamic_cast<IWindowContext*>(window.get())) {
        renderer = std::make_unique<Renderer>(renderConfig, *contextPtr);
        RendererContext(renderer.get());
    } else {
        throw std::runtime_error("Window does not implement IWindowContext!");
    }

    Register(physicsEngine.get());
    Register(assetManager.get());
    Register(meshAssetManager.get());
    Register(objImporter.get());
    Register(textureImporter.get());
    Register(shaderImporter.get());
    Register(window.get());
    Register(renderer.get());
}
