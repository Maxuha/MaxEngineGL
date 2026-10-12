//
// Created by zykov on 9/24/2026.
//

#ifndef MAXENGINE_VULKANCOMMANDBUFFERI_H
#define MAXENGINE_VULKANCOMMANDBUFFERI_H

#include <optional>
#include <vulkan/vulkan_core.h>

#include "../../../math/Rect.h"
#include "../../domain/struct/PipelineStateDesc.h"
#include "../../gl/CommandBuffer.h"
#include "../vertexbuffer/VulkanBufferManager.h"

namespace Rendering {
    enum class CommandPoolType;
    struct VulkanDynamicBuffer;
    struct VulkanPipeline;

    class VulkanCommandBufferi {
    public:
        VulkanCommandBufferi();

        explicit VulkanCommandBufferi(VkCommandBuffer commandBuffer, CommandPoolType type);

        ~VulkanCommandBufferi();

        VkCommandBuffer GetCommandBuffer() const {
            return commandBuffer;
        }

        CommandPoolType GetType() const {
            return type;
        }

        void Begin() const;

        void Begin(uint32_t colorCount, VkFormat color, VkFormat depth) const;

        void BeginRenderPass(
            Rect viewport,
            std::optional<AttachmentDescription> color,
            std::optional<AttachmentDescription> depth,
            VkImageView colorTexture,
            VkImageView depthTexture
            );

        void BindPipeline(const VulkanPipeline &pipeline) const;

        void BindVertexBuffer(const VulkanDynamicBuffer &buffer) const;

        void BindIndexBuffer(const VulkanDynamicBuffer &buffer) const;

        void BindVertexBuffer(const VulkanBuffer &buffer) const;

        void BindIndexBuffer(const VulkanBuffer &buffer) const;

        void BindDescriptorSets(VkDescriptorSet descriptorSets, VkPipelineLayout layout, uint32_t index, const uint32_t* currentOffset = nullptr) const;

        void Draw(const uint32_t vertexCount, const uint32_t instanceCount, const uint32_t firstVertex, const uint32_t firstInstance) const;

        void DrawIndexed(const uint32_t indexCount, const uint32_t instanceCount, const uint32_t firstIndex, const int32_t vertexOffset, const uint32_t firstInstance) const;

        void EndRenderPass() const;

        void End() const;

        void ExecuteCommands(const VulkanCommandBufferi& commandBuffer) const;

        void TransitionImageLayout(const VkImage image,
                                   const VkImageLayout oldLayout,
                                   const VkImageLayout newLayout,
                                   const VkImageAspectFlags aspectMask,
                                   const uint32_t mipLevelCount,
                                   const uint32_t layerCount) const;

        void CopyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size) const;

        void PipelineBarrier(VkBuffer target) const;



    private:
        VkCommandBuffer commandBuffer;
        CommandPoolType type;

        void TransitionImageLayout(VkImage image,
                                   VkImageLayout oldLayout,
                                   VkImageLayout newLayout,
                                   VkPipelineStageFlags2 srcStageMask,
                                   VkAccessFlags2 srcAccessMask,
                                   VkPipelineStageFlags2 dstStageMask,
                                   VkAccessFlags2 dstAccessMask,
                                   VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                   uint32_t mipLevelCount = 1,
                                   uint32_t layerCount = 1) const;

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

#endif //MAXENGINE_VULKANCOMMANDBUFFERI_H
