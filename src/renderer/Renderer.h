//
// Created by zykov on 4/30/2026.
//

#ifndef MAXENGINE_RENDERER_H
#define MAXENGINE_RENDERER_H
#include "IRenderDevice.h"
#include "IRenderer.h"
#include "LightRenderer.h"
#include "domain/GLTexture.h"
#include "domain/IWindowContext.h"
#include "domain/struct/MaterialData.h"
#include "domain/struct/RenderConfig.h"
#include "gl/GLRenderDevice.h"


namespace Rendering {
    class Renderer : public IRenderer {
    public:
        explicit Renderer(const RenderConfig& renderConfig, IWindowContext &context);

        ~Renderer() override;

        TextureHandle CreateTexture(void *data, TextureCreateRequest& textureCreateDesc) override;

        MaterialHandle CreateMaterial(IShader& shader,  const PipelineStateDesc &pipelineStateDesc) override;

        void MaterialSetTexture(MaterialHandle materialHandle, const std::string &slot, TextureHandle textureId) override;

        void UpdateMaterialData(MaterialHandle handle, size_t offset, size_t size, void* data) override;

        void UpdateTextureFilterMode(TextureHandle& textureId, TextureWrap textureWrap) override;

        void UpdateBufferData(BufferHandle bufferId, size_t offset, size_t size, void *data) override;

        Mesh *CreateMesh(std::span<const Vertex> vertices, std::span<const uint32_t> indices) override;

        void BeginFrame(Camera &camera) override;

        void Submit(Scene &scene) override;

        void Render() override;

        void EndFrame() override;

        Shader* GetLitShader() const override;

    private:
        Camera *activeCamera{};
        Shader* litShader;

        IRenderDevice *renderDevice;
        ICommandBuffer *commandBuffer;
        ILightRenderer *lightRenderer{};
        MeshManager *meshManager{};

        PipelineHandle shadowPipeline{};

        TextureHandle depthTexture{};

        BufferHandle cameraBufferId{};
        BufferHandle depthBuffer{};

        std::vector<MaterialData> materials;
    };
} // namespace Rendering


#endif //MAXENGINE_RENDERER_H
