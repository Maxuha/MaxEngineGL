#include "DIContainer.h"

#include "../GLFWWindowImpl.h"
#include "../OpenGLGLFWGraphicsContext.h"
#include "../VulkanGLFWGraphicsContext.h"
#include "../IO/ModelImporter.h"
#include "../renderer/gl/OGLRenderDevice.h"
#include "../renderer/Renderer.h"
#include "../IO/AssimpImporter.h"
#include "../IO/FileReader.h"
#include "../renderer/gl/OGLGraphicsContext.h"
#include "../src/InputController.h"

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

    WindowDesc windowDesc;
    windowDesc.width = 2160;
    windowDesc.height = 1440;
    window = std::make_unique<GLFWWindowImpl>(windowDesc, new VulkanGLFWGraphicsContext);

    // Shader* standardLitShader = shaderImporter->Import("phong/phong");
    // standardLitShader->AddProperty(ShaderProperty::SHININESS, ShaderMetaProperty(0, sizeof(float)));
    // standardLitShader->AddProperty(ShaderProperty::COLOR_DIFFUSE, ShaderMetaProperty(16, sizeof(Vector4)));
  //  Shader* standardShadowShader = shaderImporter->Import("depth/depth");

    RenderConfig renderConfig;
    renderConfig.api = RenderAPI::Vulkan;
  //  renderConfig.litShader = standardLitShader;
  //  renderConfig.shadowShader = standardShadowShader;

    renderer = std::make_unique<Renderer>(renderConfig, *window);

    inputController = std::make_unique<InputController>(dynamic_cast<IInputContext *>(window.get()));

    Register(physicsEngine.get());
    Register(assetManager.get());
    Register(meshAssetManager.get());
    Register(objImporter.get());
    Register(textureImporter.get());
    Register(shaderImporter.get());
    Register(window.get());
    Register(renderer.get());
    Register(graphicsContext.get());
    Register(inputController.get());
}
