//
// Created by zykov on 4/30/2026.
//

#ifndef MAXENGINE_RENDERER_H
#define MAXENGINE_RENDERER_H
#include "IRenderDevice.h"
#include "IRenderer.h"
#include "IRenderPass.h"
#include "LightRenderer.h"
#include "../IWindow.h"
#include "domain/CameraRender.h"
#include "domain/LightRenderData.h"
#include "domain/Texture.h"
#include "domain/struct/MaterialData.h"
#include "domain/struct/RenderConfig.h"
#include "gl/OGLRenderDevice.h"
#include "vulkan/VulkanFrameManager.h"
#include "vulkan/commandbuffer/VulkanCommandBufferManager.h"
#include "vulkan/pipeline/VulkanPipelineManager.h"


namespace Rendering {
    class IFence;
    struct FrameBufferHandle;

    struct RenderPass {
        std::vector<AttachmentDescription> colorAttachments = {};

        Rect viewport = Rect{0, 0, 2160, 1440};
        Color clearColor = Color{0.5f, 0.5f, 0.5f, 1.0f};
    };


    struct GraphicsShader {
        PipelineHandle pipeline;
        ResourceSetLayoutHandle resourceSetLayout;
    };

    class Renderer : public IRenderer {
    public:
        explicit Renderer(const RenderConfig& renderConfig, IWindow& window);

        ~Renderer() override;

        /// Start Factory

        CameraRender* CreateCamera(float fov, float zNear, float zFar, float aspectRatio) override;

        TextureHandle CreateTexture(void *data, TextureCreateRequest& textureCreateDesc) override;

        MaterialHandle CreateMaterial(const ShaderHandle shaderHandle, const MaterialDesc &materialDesc) override;

        ShaderHandle CreateShader(const ShaderDesc& shaderDesc) override;

        LightHandle AddLight(Light& light) override;

        LightHandle AddLight(LightType type) override;

        /// End Factory

        void BindTextureToMaterial(const Material& material, const uint32_t slot, const Texture& texture) override;

        void BindBufferToMaterial(MaterialHandle materialHandle, const uint32_t slot,
            BufferHandle bufferId) override;

        void UpdateMaterialData(MaterialHandle handle, size_t offset, size_t size, void* data) override;

        void UpdateTextureFilterMode(TextureHandle& textureId, TextureWrap textureWrap) override;

        void UpdateBufferData(BufferHandle bufferId, size_t offset, size_t size, void *data) override;

        Mesh *CreateMesh(const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices) override;

        void BeginFrame(Camera &camera) override;

        void Submit(Scene &scene) override;

        void Render(Scene &scene) override;

        void Render(const std::vector<DrawCall> &drawCalls) override;

        void EndFrame() override;

        Shader* GetLitShader() const override;

    private:

        Camera *activeCamera{};
        Shader* litShader;

        IRenderDevice *renderDevice;
        CommandBuffer *commandBuffer;
        ILightRenderer *lightRenderer{};
        MeshManager *meshManager{};

        PipelineHandle shadowPipeline{};

        TextureHandle depthTexture{};

        BufferHandle cameraBufferId{};
        BufferHandle depthBuffer{};

        std::vector<IRenderPass*> renderPasses;
        std::vector<RenderPass> renderPassesDesc;

        std::vector<CameraRenderData> cameras;
        std::vector<MaterialData> materials;
        std::vector<TextureData> textures;
        std::vector<GraphicsShader> shaders;

        VertexLayout vertexLayout;
        Rect viewport;

        LightRenderData lightRenderData;
        std::vector<LightItemData> lights;

        ImageFormat depthFormat;

        ResourceSetLayoutHandle cameraAndLightSetLayout{};
        ResourceSetLayoutHandle modelSetLayout{};
    };
} // namespace Rendering


#endif //MAXENGINE_RENDERER_H
