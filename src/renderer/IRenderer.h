//
// Created by zykov on 4/30/2026.
//

#ifndef MAXENGINE_IRENDERER_H
#define MAXENGINE_IRENDERER_H

#include "../Scene.h"
#include "domain/Mesh.h"
#include "domain/struct/PipelineStateDesc.h"
#include "gl/desc/TextureCreateRequest.h"


class Camera;
class MaterialHandle;

namespace Rendering {
    class IRenderer {
    public:
        virtual ~IRenderer() = default;

        virtual TextureHandle CreateTexture(void *data, TextureCreateRequest& textureCreateDesc) = 0;

        virtual MaterialHandle CreateMaterial(IShader& shader, const PipelineStateDesc &pipelineStateDesc) = 0;

        virtual void MaterialSetTexture(MaterialHandle materialHandle, const std::string &slot, TextureHandle textureId) = 0;

        virtual void UpdateMaterialData(MaterialHandle handle, size_t offset, size_t size, void* data) = 0;

        virtual void UpdateTextureFilterMode(TextureHandle& textureId, TextureWrap textureWrap) = 0;

        virtual void UpdateBufferData(BufferHandle bufferId, size_t offset, size_t size, void* data) = 0;

        virtual Mesh *CreateMesh(std::span<const Vertex> vertices, std::span<const uint32_t> indices) = 0;

        virtual void Submit(Scene &scene) = 0;

        virtual void BeginFrame(Camera &camera) = 0;

        virtual void Render() = 0;

        virtual void EndFrame() = 0;

        virtual Shader* GetLitShader() const = 0;
    };
} // namespace Rendering


#endif //MAXENGINE_IRENDERER_H
