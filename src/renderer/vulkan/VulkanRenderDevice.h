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
#include "../domain/buffer/BufferDesc.h"

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

        CommandBuffer *AllocateCommandBuffer(CommandBufferType type) override;

        TextureHandle CreateTexture(const TextureCreateRequest &desc, const void *initialData) override;

        PipelineLayoutHandle CreatePipelineLayout(const std::vector<ResourceSetLayoutHandle> &resourceSetLayouts) override;

        ResourceSetLayoutHandle CreateResourceSetLayout(const ResourceSetLayoutDesc &resourceSetLayoutDesc) override;

        ResourceSetHandle CreateResourceSet(const ResourceSetLayoutHandle resourceSetLayoutHandle) override;

        void UpdateResourceSet(ResourceSetHandle setHandle, const uint32_t binding, const TextureHandle textureHandle) override;

        void UpdateResourceSet(ResourceSetHandle setHandle, const uint32_t binding, const BufferHandle bufferHandle) override;

        BufferHandle CreateBuffer(const BufferDesc &desc) override;

        void UpdateBuffer(BufferHandle id, const void *data, int offset, int size) override;

        void DestroyBuffer(BufferHandle id) override;

        PipelineHandle
        CreatePipeline(const ShaderDesc &shaderDesc, const PipelineStateDesc &pipelineStateDesc) override;

        void SubmitCommandBuffer(CommandBuffer *commandBuffer) override;

        void Present() override;

        TextureFormat GetDisplayFormat() override;

    private:
        int MAX_FRAMES_IN_FLIGHT = 3;

        VulkanCommandBufferi mainCommandBuffer;
        VulkanCommandBufferi loaderCommandBuffer;


        VulkanInstance *instance;
        VulkanSurface *surface;
        VulkanPhysicalDevice *physicalDevice;
        VulkanLogicalDevice *logicalDevice;
        VulkanShaderManager *shaderManager;
        VulkanPipelineManager *pipelineManager;
        VulkanCommandBufferManager *commandBufferManager;
        VulkanTextureManager *textureManager;
        VulkanBufferManager *bufferManager;
        VulkanFrameManager *frameManager;
        VulkanDescriptorSetManager *descriptorSetManager;
        VulkanStagingBufferPool *stagingBufferPool;
        VulkanSwapChain *swapChain;

        VulkanTexture depthTexture;

        std::vector<VulkanDynamicBuffer> dynamicBuffers;
        std::vector<VulkanBuffer> buffers;
        std::vector<GraphicsVulkanShader> shaders;
        std::unordered_map<ResourceSetLayoutDesc, VkDescriptorSetLayout> descriptorSetLayouts;
        std::vector<VulkanDescriptorSet> descriptorSets;

        TextureFormat MapVkFormat(const VkFormat format) {
            switch (format) {
                case VK_FORMAT_R8G8B8A8_UNORM: {
                    return TextureFormat::RGBA;
                }
                case VK_FORMAT_R8G8B8A8_SRGB: {
                    return TextureFormat::SRGBA;
                }
                case VK_FORMAT_B8G8R8A8_SRGB: {
                    return TextureFormat::SRGBA;
                }
                default: {
                    return TextureFormat::RGBA;
                }
            }
        }

        VkFormat MapVkFormat(const TextureFormat format) {
            switch (format) {
                case TextureFormat::RGBA: {
                    return VK_FORMAT_R8G8B8A8_UNORM;
                }
                case TextureFormat::SRGBA: {
                    return VK_FORMAT_R8G8B8A8_SRGB;
                }
                case TextureFormat::DEPTH: {
                    return VK_FORMAT_D32_SFLOAT;
                }
                case TextureFormat::RGB:
                    return VK_FORMAT_R8G8B8_UNORM;
                    break;
                case TextureFormat::UNDEFINED:
                    return VK_FORMAT_UNDEFINED;
                    break;
            }
        }

        constexpr VkAttachmentLoadOp MapAttachmentLoadOp(const AttachmentLoadOp op) {
            switch (op) {
                case AttachmentLoadOp::LOAD:      return VK_ATTACHMENT_LOAD_OP_LOAD;
                case AttachmentLoadOp::CLEAR:     return VK_ATTACHMENT_LOAD_OP_CLEAR;
                case AttachmentLoadOp::DONT_CARE: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            }
            return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        }

        constexpr VkAttachmentStoreOp MapAttachmentStoreOp(const AttachmentStoreOp  op) {
            switch (op) {
                case AttachmentStoreOp::STORE:     return VK_ATTACHMENT_STORE_OP_STORE;
                case AttachmentStoreOp::DONT_CARE: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
            }
            return VK_ATTACHMENT_STORE_OP_DONT_CARE;
        }

        VkRenderingAttachmentInfo CreateAttachmentInfo(const AttachmentDescription& description, const VkImageView target) {
            VkRenderingAttachmentInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            info.loadOp = MapAttachmentLoadOp(description.loadOp);
            info.storeOp = MapAttachmentStoreOp(description.storeOp);
            info.imageView = target;

            const VkClearColorValue clearColor = {description.clearColor.r, description.clearColor.g, description.clearColor.b, description.clearColor.a} ;

            switch (description.type) {
                case AttachmentType::COLOR:
                    info.clearValue.color = clearColor;
                    info.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    break;
                case AttachmentType::DEPTH:
                    info.clearValue.depthStencil = {description.depthClearValue, 0};
                    info.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
                    break;
                case AttachmentType::STENCIL:
                    break;
                case AttachmentType::NONE:
                    break;
            }
            return info;
        }
    };
} // Rendering

#endif //MAXENGINE_VULKANRENDERDEVICE_H
