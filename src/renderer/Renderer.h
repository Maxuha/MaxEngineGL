//
// Created by zykov on 4/30/2026.
//

#ifndef MAXENGINE_RENDERER_H
#define MAXENGINE_RENDERER_H
#include "IRenderDevice.h"
#include "IRenderer.h"
#include "LightRenderer.h"
#include "RenderQueue.h"
#include "domain/GLTexture.h"
#include "domain/IWindowContext.h"
#include "domain/buffer/frame/FrameBuffer.h"
#include "domain/struct/RenderConfig.h"
#include "pass/IRenderPass.h"


namespace Rendering {
    class Renderer : public IRenderer {
    public:
        explicit Renderer(const RenderConfig& renderConfig, IWindowContext &context);

        ~Renderer() override;

        PipelineHandle CreatePipeline(const std::vector<ShaderInput> &shaders, const RenderState &renderState) override;

        TextureId CreateTexture(void *data, TextureCreateRequest& textureCreateDesc) override;

         void UpdateTextureFilterMode(TextureId& textureId, TextureWrap textureWrap) override;

        Mesh *CreateMesh(std::span<const Vertex> vertices, std::span<const uint32_t> indices) override;

        void BeginFrame(Camera &camera, Shader *shader) override;

        void Submit(Scene &scene) override;

        void Render() override;

        void EndFrame() override;

    private:
        Camera *activeCamera{};

        std::vector<IRenderPass *> renderPasses;

        RenderQueue *renderQueue;
        IRenderDevice *renderDevice;
        ICommandBuffer *commandBuffer;
        LightRenderer *lightRenderer{};
        MeshManager *meshManager{};
        Shader *mainShader;

        GLTexture *depthTexture{};

        BufferId cameraBufferId{};
        FrameBuffer *depthBuffer{};
    };
} // namespace Rendering


#endif //MAXENGINE_RENDERER_H
