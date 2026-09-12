//
// Created by zykov on 6/16/2026.
//

#ifndef MAXENGINE_VULKANRENDERDEVICE_H
#define MAXENGINE_VULKANRENDERDEVICE_H

#include "VulkanDescriptorSetManager.h"
#include "VulkanLogicalDevice.h"
#include "../IRenderDevice.h"
#include "../src/renderer/RenderContext.h"
#include "texture/VulkanTextureManager.h"
#include "vertexbuffer/VulkanBufferManager.h"
#include "vertexbuffer/VulkanStagingBufferPool.h"

namespace Rendering {
    struct DescriptorLayout {
        VkDescriptorSetLayout layout;
        std::vector<VkDescriptorSetLayoutBinding> bindings;
    };

    class VulkanSurface;
    class VulkanInstance;
    class VulkanShaderManager;
    class VulkanPipelineManager;
    class VulkanFrameBufferManager;
    class VulkanRenderPassManager;
    class VulkanCommandPoolManager;
    class VulkanCommandBufferManager;
    class VulkanTextureManager;
    class VulkanQueueFamily;
    struct GraphicsVulkanShader;
    struct VulkanDescriptorSet;

    class VulkanRenderDevice : public IRenderDevice {
    public:
        explicit VulkanRenderDevice(IWindow &window);

        ~VulkanRenderDevice() override;

        CommandBuffer *AllocateCommandBuffer() override;

        TextureHandle CreateTexture(const TextureCreateRequest &desc, const void *initialData) override;

        ResourceSetLayoutHandle CreateResourceSetLayout(const ResourceSetLayoutDesc &resourceSetLayoutDesc) override;

        ResourceSetHandle CreateResourceSet(const PipelineHandle pipelineHandle, const ResourceBinding resource) override;

        ResourceSetHandle CreateResourceSet(const ResourceSetLayoutHandle resourceSetLayoutHandle) override;

        void UpdateResourceSet(ResourceSetHandle setHandle, const uint32_t binding, const TextureHandle textureHandle) override;

        void UpdateResourceSet(ResourceSetHandle setHandle, const uint32_t binding, const BufferHandle bufferHandle) override;

        BufferHandle CreateBuffer(const BufferDesc &desc, const void *data) override;

        BufferHandle CreateFrameBuffer(TextureHandle texture) override;

        void UpdateBuffer(BufferHandle id, const void *data, int offset, int size) override;

        void DestroyBuffer(BufferHandle id) override;

        void BindUniformBuffer(BufferHandle id, uint32_t index) override;

        PipelineHandle CreatePipeline(const std::vector<ShaderHandle> &shaders, const RenderPassHandle renderPass, const PipelineStateDesc &pipelineStateDesc) override;

        PipelineHandle CreatePipeline(const ShaderHandle shader, const PipelineStateDesc &pipelineStateDesc) override;

        PipelineHandle CreatePipeline(const ShaderDesc &shaderDesc, const PipelineStateDesc &pipelineStateDesc) override;

        void SubmitCommandBuffer(CommandBuffer *commandBuffer) override;

        void Present() override;

        ImageFormat GetDisplayFormat() override;

    private:
        int MAX_FRAMES_IN_FLIGHT = 2;

        VulkanInstance *instance;
        VulkanSurface *surface;
        VulkanPhysicalDevice *physicalDevice;
        VulkanLogicalDevice *logicalDevice;
        VulkanShaderManager *shaderManager;
        VulkanPipelineManager *pipelineManager;
        VulkanFrameBufferManager *frameBufferManager;
        VulkanCommandPoolManager *commandPoolManager;
        VulkanCommandBufferManager *commandBufferManager;
        VulkanTextureManager *textureManager;
        VulkanBufferManager *bufferManager;
        VulkanFrameManager *frameManager;
        VulkanDescriptorSetManager* descriptorSetManager;
        VulkanStagingBufferPool* stagingBufferPool;
        VulkanRenderPassManager* renderPassManager;
        VulkanSwapChain* swapChain;

        VulkanQueueFamily *queueFamily;
        VulkanRenderPassManager *renderPass;

        VulkanTexture depthTexture;

        std::vector<VulkanVertexBuffer> buffers;
        std::vector<GraphicsVulkanShader> shaders;
        std::unordered_map<ResourceSetLayoutDesc, VkDescriptorSetLayout> descriptorSetLayouts;
        std::vector<VulkanDescriptorSet> descriptorSets;


        ImageFormat MapVkFormat(const VkFormat format) {
            switch (format) {
                case VK_FORMAT_R8G8B8A8_UNORM: {
                    return ImageFormat::BGRA8_Srgb;
                }
                case VK_FORMAT_R8G8B8A8_SRGB: {
                    return ImageFormat::RGBA8_Srgb;
                }
                case VK_FORMAT_B8G8R8A8_SRGB: {
                    return ImageFormat::BGRA8_Srgb;
                }
                default: {
                    return ImageFormat::Undefined;
                }
            }
        }
    };
} // Rendering

#endif //MAXENGINE_VULKANRENDERDEVICE_H
