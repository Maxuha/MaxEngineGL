//
// Created by zykov on 4/30/2026.
//

#ifndef MAXENGINE_IRENDERER_H
#define MAXENGINE_IRENDERER_H

#include "../Scene.h"
#include "domain/CameraRender.h"
#include "domain/LightRenderData.h"
#include "domain/Mesh.h"
#include "domain/Texture.h"
#include "domain/struct/PipelineStateDesc.h"
#include "gl/desc/TextureCreateRequest.h"

class MaterialHandle;

namespace Rendering {

    struct CameraRenderData {
        BufferHandle ViewProjectionBuffer;
        ResourceSetHandle ResourceSet;
    };

    class IRenderer {
    public:
        virtual ~IRenderer() = default;

        virtual CameraRender* CreateCamera(float fov, float zNear, float zFar, float aspectRatio) = 0;

        virtual TextureHandle CreateTexture(void *data, TextureCreateRequest& textureCreateDesc) = 0;

        virtual MaterialHandle CreateMaterial(const ShaderHandle shader, const MaterialDesc &materialDesc) = 0;

        virtual ShaderHandle CreateShader(const ShaderDesc& shaderDesc) = 0;

        virtual LightHandle AddLight(Light& light) = 0;

        virtual LightHandle AddLight(LightType type) = 0;

        //material

        virtual void BindTextureToMaterial(const Material& material, const uint32_t slot, const Texture& texture) = 0;

        virtual void BindBufferToMaterial(MaterialHandle materialHandle, const uint32_t slot, BufferHandle bufferId) = 0;

        virtual void UpdateMaterialData(MaterialHandle handle, size_t offset, size_t size, void* data) = 0;

        virtual void UpdateTextureFilterMode(TextureHandle& textureId, TextureWrap textureWrap) = 0;

        virtual void UpdateBufferData(BufferHandle bufferId, size_t offset, size_t size, void* data) = 0;

        virtual Mesh *CreateMesh(const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices) = 0;

        // rendering

        virtual void Submit(Scene &scene) = 0;

        virtual void BeginFrame(Camera &camera) = 0;

        virtual void Render(Scene &scene) = 0;

        virtual void Render(const std::vector<DrawCall> &drawCalls) = 0;

        virtual void EndFrame() = 0;

        virtual Shader* GetLitShader() const = 0;
    };
} // namespace Rendering


#endif //MAXENGINE_IRENDERER_H
