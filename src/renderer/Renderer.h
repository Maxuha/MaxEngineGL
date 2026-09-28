//
// Created by zykov on 4/30/2026.
//

#ifndef MAXENGINE_RENDERER_H
#define MAXENGINE_RENDERER_H
#include "IRenderDevice.h"
#include "IRenderer.h"
#include "IRenderPass.h"
#include "../IWindow.h"
#include "domain/CameraRender.h"
#include "domain/LightRenderData.h"
#include "domain/Texture.h"
#include "domain/struct/MaterialData.h"
#include "domain/struct/RenderConfig.h"
#include "gl/OGLRenderDevice.h"
#include "vulkan/pipeline/VulkanPipelineManager.h"


namespace Rendering {
    class IFence;
    struct FrameBufferHandle;

    struct RenderPass {
        std::optional<AttachmentDescription> colorAttachment = {};
        std::optional<AttachmentDescription> depthAttachment = {};

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

        MaterialHandle CreateMaterial(ShaderHandle shaderHandle, const MaterialDesc &materialDesc) override;

        ShaderHandle CreateShader(const ShaderDesc& shaderDesc) override;

        Mesh *CreateMesh(const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices) override;

        LightHandle CreateLight(LightType type) override;

        /// End Factory

        /// Update data

        void BindTextureToMaterial(const MaterialHandle material, uint32_t slot, const TextureHandle texture) const;

        void BindTextureToMaterial(const Material& material, uint32_t slot, const Texture& texture) override;

        void BindBufferToMaterial(MaterialHandle materialHandle, uint32_t slot, BufferHandle bufferId) override;

        /// End Update data

        /// Render

        void BeginFrame(Camera &camera) override;

        void Render(Scene &scene) override;

        void EndFrame() override;

        /// End Render

        /// Config

        void SetDepthShader( Shader& shader);

    private:

        Camera *activeCamera{};
        Shader* litShader;
        Shader* depthShader;

        IRenderDevice *renderDevice;
        CommandBuffer *commandBuffer;
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
        ShadowRenderData shadowRenderData;

        LightItemData ambientLight;
        LightItemData directionalLight;
        std::vector<LightItemData> pointLights;
        std::vector<LightItemData> spotLights;

        std::vector<LightItemData> lights;
        uint32_t lastPointLightIndex = 0;
        uint32_t lastSpotLightIndex = 0;

        TextureFormat depthFormat;

        CameraRenderData lightSpace;

        ResourceSetLayoutHandle cameraAndLightSetLayout{};
        ResourceSetLayoutHandle modelSetLayout{};
        PipelineLayoutHandle mainPipelineLayout{};

    };
} // namespace Rendering


#endif //MAXENGINE_RENDERER_H
