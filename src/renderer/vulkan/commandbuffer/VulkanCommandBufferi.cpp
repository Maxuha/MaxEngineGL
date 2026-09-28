//
// Created by zykov on 9/24/2026.
//

#include "VulkanCommandBufferi.h"

namespace Rendering {
    VulkanCommandBufferi::VulkanCommandBufferi(const VkCommandBuffer commandBuffer) : commandBuffer(commandBuffer) {
    }

    VulkanCommandBufferi::~VulkanCommandBufferi() {
    }

    void VulkanCommandBufferi::transitionImageLayout(const VkImage image, const VkImageLayout oldLayout,
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

        transitionImageLayout(image, oldLayout, newLayout,
                              srcStageMask, srcAccessMask,
                              dstStageMask, dstAccessMask,
                              aspectMask, mipLevelCount, layerCount);
    }

    void VulkanCommandBufferi::transitionImageLayout(const VkImage image, const VkImageLayout oldLayout,
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