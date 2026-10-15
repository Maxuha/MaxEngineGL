//
// Created by zykov on 9/24/2026.
//

#include "VulkanCommandBufferi.h"

#include "../pipeline/VulkanPipelineManager.h"
#include "../src/renderer/vulkan/vertexbuffer/VulkanBufferManager.h"

namespace Rendering {
    VulkanCommandBufferi::VulkanCommandBufferi() {
    }

    VulkanCommandBufferi::VulkanCommandBufferi(const VkCommandBuffer commandBuffer, const CommandPoolType type) : commandBuffer(commandBuffer), type(type) {
        assert(commandBuffer != VK_NULL_HANDLE);
        vkResetCommandBuffer(commandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    }

    VulkanCommandBufferi::~VulkanCommandBufferi() {
       // vkEndCommandBuffer(commandBuffer);
    }

    void VulkanCommandBufferi::Begin() const {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        vkBeginCommandBuffer(commandBuffer, &beginInfo);
    }

    void VulkanCommandBufferi::Begin(const uint32_t colorCount, const VkFormat color, const VkFormat depth) const {
        VkCommandBufferInheritanceRenderingInfo inheritanceRenderingInfo{};
        inheritanceRenderingInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_RENDERING_INFO;
        inheritanceRenderingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        inheritanceRenderingInfo.colorAttachmentCount = colorCount;
        inheritanceRenderingInfo.pColorAttachmentFormats = &color;
        inheritanceRenderingInfo.depthAttachmentFormat = depth;
        inheritanceRenderingInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;
        VkCommandBufferInheritanceInfo inheritanceInfo{};
        inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
        inheritanceInfo.pNext = &inheritanceRenderingInfo;
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
        beginInfo.pInheritanceInfo = &inheritanceInfo;
        vkBeginCommandBuffer(commandBuffer, &beginInfo);
    }

    void VulkanCommandBufferi::BeginRenderPass(const Rect viewport, std::optional<AttachmentDescription> color,
                                               std::optional<AttachmentDescription> depth, VkImageView colorTexture,
                                               VkImageView depthTexture) {

        std::vector<VkRenderingAttachmentInfo> colorAttachments;
        VkRenderingAttachmentInfo depthAttachment;

        if (color.has_value()) {
            AttachmentDescription attachmentDescription = color.value();

            VkRenderingAttachmentInfo colorAttachment = CreateAttachmentInfo(attachmentDescription, colorTexture);

            colorAttachments.push_back(colorAttachment);
        }

        if (depth.has_value()) {
            AttachmentDescription attachmentDescription = depth.value();

            depthAttachment = CreateAttachmentInfo(attachmentDescription, depthTexture);
        }

        VkRenderingInfo renderingInfo{};
        renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderingInfo.flags = VK_RENDERING_CONTENTS_SECONDARY_COMMAND_BUFFERS_BIT;
        renderingInfo.renderArea = {static_cast<int32_t>(viewport.x), static_cast<int32_t>(viewport.y), static_cast<uint32_t>(viewport.width), static_cast<uint32_t>(viewport.height)};
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = colorAttachments.size();
        renderingInfo.pColorAttachments = colorAttachments.data();
        renderingInfo.pDepthAttachment = &depthAttachment;
        renderingInfo.pStencilAttachment = nullptr;

        vkCmdBeginRendering(commandBuffer, &renderingInfo);
    }

    void VulkanCommandBufferi::BindPipeline(const VulkanPipeline &pipeline) const {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.Pipeline);
    }

    void VulkanCommandBufferi::BindVertexBuffer(const VulkanDynamicBuffer &buffer) const {
        const VkBuffer vertexBuffers[] = {buffer.buffers[0]};
        constexpr VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    }

    void VulkanCommandBufferi::BindIndexBuffer(const VulkanDynamicBuffer &buffer) const {
        VkBuffer indexBuffer = buffer.buffers[0];
        VkDeviceSize offset = 0;

        vkCmdBindIndexBuffer(commandBuffer, indexBuffer, offset, VK_INDEX_TYPE_UINT16);
    }

    void VulkanCommandBufferi::BindVertexBuffer(const VulkanBuffer &buffer) const {
        const VkBuffer vertexBuffers[] = {buffer.buffer};
        constexpr VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    }

    void VulkanCommandBufferi::BindIndexBuffer(const VulkanBuffer &buffer) const {
        const VkBuffer indexBuffer = buffer.buffer;
        constexpr VkDeviceSize offset = 0;
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer, offset, VK_INDEX_TYPE_UINT16);
    }

    void VulkanCommandBufferi::BindDescriptorSets(const VkDescriptorSet descriptorSets, const VkPipelineLayout layout, const uint32_t index, const uint32_t* currentOffset) const {
        uint32_t dynamicOffsetCount = 0;

        // if (currentOffset != nullptr || *currentOffset != 0) {
        //     dynamicOffsetCount = 1;
        // }

        if (index == 1) {
            dynamicOffsetCount = 1;
        }

        vkCmdBindDescriptorSets(
                 commandBuffer,
                 VK_PIPELINE_BIND_POINT_GRAPHICS,
                 layout,
                 index,
                 1,
                 &descriptorSets,
                 0,
                 nullptr
             );
    }

    void VulkanCommandBufferi::EndRenderPass() const {
        vkCmdEndRendering(commandBuffer);
    }

    void VulkanCommandBufferi::End() const {
        vkEndCommandBuffer(commandBuffer);
    }

    void VulkanCommandBufferi::ExecuteCommands(const VulkanCommandBufferi &commandBuffer) const {
        vkCmdExecuteCommands(this->commandBuffer, 1, &commandBuffer.commandBuffer);
    }

    void VulkanCommandBufferi::Draw(const uint32_t vertexCount, const uint32_t instanceCount,
                                    const uint32_t firstVertex, const uint32_t firstInstance) const {
        vkCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
    }

    void VulkanCommandBufferi::DrawIndexed(const uint32_t indexCount, const uint32_t instanceCount,
        const uint32_t firstIndex, const int32_t vertexOffset, const uint32_t firstInstance) const {
        vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }

    void VulkanCommandBufferi::TransitionImageLayout(const VkImage image, const VkImageLayout oldLayout,
                                                     const VkImageLayout newLayout, const VkImageAspectFlags aspectMask,
                                                     const uint32_t mipLevelCount,
                                                     const uint32_t layerCount) const {
        VkPipelineStageFlags2 srcStageMask = VK_PIPELINE_STAGE_2_NONE;
        VkAccessFlags2 srcAccessMask = VK_ACCESS_2_NONE;
        VkPipelineStageFlags2 dstStageMask = VK_PIPELINE_STAGE_2_NONE;
        VkAccessFlags2 dstAccessMask = VK_ACCESS_2_NONE;

        // --- Source masks (What to wait for) ---
        switch (oldLayout) {
            case VK_IMAGE_LAYOUT_UNDEFINED:
            //    srcStageMask = VK_PIPELINE_STAGE_2_NONE;
                srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
               // srcAccessMask = VK_ACCESS_2_NONE;
                srcAccessMask = srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;;
                break;

            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
                srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
                srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
                srcStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT |
                               VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
                srcAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT | VK_PIPELINE_STAGE_2_BLIT_BIT;
                srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
                srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT | VK_PIPELINE_STAGE_2_BLIT_BIT;
                srcAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;
                break;

            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
                srcStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
                srcAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
                break;

            case VK_IMAGE_LAYOUT_GENERAL:
                srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
                srcAccessMask = VK_ACCESS_2_SHADER_WRITE_BIT | VK_ACCESS_2_SHADER_READ_BIT;
                break;

            case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
                srcStageMask = VK_PIPELINE_STAGE_2_NONE;
                srcAccessMask = VK_ACCESS_2_NONE;
                break;

            default:
                break;
        }

        // --- Destination masks (What to block) ---
        switch (newLayout) {
            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
                dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
                dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT |
                               VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
                dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                                VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                dstStageMask = VK_PIPELINE_STAGE_2_COPY_BIT | VK_PIPELINE_STAGE_2_BLIT_BIT;
                dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
                dstStageMask = VK_PIPELINE_STAGE_2_COPY_BIT | VK_PIPELINE_STAGE_2_BLIT_BIT;
                dstAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;
                break;

            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
                dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
                dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
                break;

            case VK_IMAGE_LAYOUT_GENERAL:
                dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
                dstAccessMask = VK_ACCESS_2_SHADER_WRITE_BIT | VK_ACCESS_2_SHADER_READ_BIT;
                break;

            case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
                dstStageMask = VK_PIPELINE_STAGE_2_NONE;
                dstAccessMask = VK_ACCESS_2_NONE;
                break;

            default:
                break;
        }

        TransitionImageLayout(image, oldLayout, newLayout,
                              srcStageMask, srcAccessMask,
                              dstStageMask, dstAccessMask,
                              aspectMask, mipLevelCount, layerCount);
    }

    void VulkanCommandBufferi::CopyBuffer(const VkBuffer src, const VkBuffer dst, const VkDeviceSize size, const VkDeviceSize offset) const {
        VkBufferCopy region{};
        region.size = size;
        region.srcOffset = 0;
        region.dstOffset = offset;
        vkCmdCopyBuffer(commandBuffer, src, dst, 1, &region);
    }

    void VulkanCommandBufferi::PipelineBarrier(const VkBuffer target) const {
        VkBufferMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.buffer = target;
        barrier.offset = 0;
        barrier.size = VK_WHOLE_SIZE;

        vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
            0, 0, nullptr, 1, &barrier, 0, nullptr
        );
    }

    void VulkanCommandBufferi::TransitionImageLayout(const VkImage image, const VkImageLayout oldLayout,
                                                     const VkImageLayout newLayout,
                                                     const VkPipelineStageFlags2 srcStageMask,
                                                     const VkAccessFlags2 srcAccessMask,
                                                     const VkPipelineStageFlags2 dstStageMask,
                                                     const VkAccessFlags2 dstAccessMask,
                                                     const VkImageAspectFlags aspectMask, const uint32_t mipLevelCount,
                                                     const uint32_t layerCount) const {
        VkImageMemoryBarrier2 imageBarrier{};
        imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        imageBarrier.pNext = nullptr;

        // Define synchronization scopes and access masks directly inside the barrier
        imageBarrier.srcStageMask = srcStageMask;
        imageBarrier.srcAccessMask = srcAccessMask;
        imageBarrier.dstStageMask = dstStageMask;
        imageBarrier.dstAccessMask = dstAccessMask;

        // Layout transitions
        imageBarrier.oldLayout = oldLayout;
        imageBarrier.newLayout = newLayout;

        // Keep ownership on the same queue family (change these if doing a queue transfer)
        imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        // Target image and its subresource range
        imageBarrier.image = image;
        imageBarrier.subresourceRange.aspectMask = aspectMask;
        imageBarrier.subresourceRange.baseMipLevel = 0;
        imageBarrier.subresourceRange.levelCount = mipLevelCount;
        imageBarrier.subresourceRange.baseArrayLayer = 0;
        imageBarrier.subresourceRange.layerCount = layerCount;

        // Wrap the barrier into the Dependency Info structure
        VkDependencyInfo dependencyInfo{};
        dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        dependencyInfo.pNext = nullptr;
        dependencyInfo.dependencyFlags = 0; // Can be VK_DEPENDENCY_BY_REGION_BIT for render passes
        dependencyInfo.memoryBarrierCount = 0;
        dependencyInfo.pMemoryBarriers = nullptr;
        dependencyInfo.bufferMemoryBarrierCount = 0;
        dependencyInfo.pBufferMemoryBarriers = nullptr;
        dependencyInfo.imageMemoryBarrierCount = 1;
        dependencyInfo.pImageMemoryBarriers = &imageBarrier;

        // Record the pipeline barrier command
        vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
    }

} // Rendering