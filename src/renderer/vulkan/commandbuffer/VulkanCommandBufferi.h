//
// Created by zykov on 9/24/2026.
//

#ifndef MAXENGINE_VULKANCOMMANDBUFFERI_H
#define MAXENGINE_VULKANCOMMANDBUFFERI_H

#include <vulkan/vulkan_core.h>

namespace Rendering {
    class VulkanCommandBufferi {
    public:
        explicit VulkanCommandBufferi(VkCommandBuffer commandBuffer);

        ~VulkanCommandBufferi();

        VkCommandBuffer GetCommandBuffer() const {
            return commandBuffer;
        }

        void transitionImageLayout(const VkImage image,
                                   const VkImageLayout oldLayout,
                                   const VkImageLayout newLayout,
                                   const VkImageAspectFlags aspectMask,
                                   const uint32_t mipLevelCount,
                                   const uint32_t layerCount) const;

    private:
        VkCommandBuffer commandBuffer;

        void transitionImageLayout(VkImage image,
                                   VkImageLayout oldLayout,
                                   VkImageLayout newLayout,
                                   VkPipelineStageFlags2 srcStageMask,
                                   VkAccessFlags2 srcAccessMask,
                                   VkPipelineStageFlags2 dstStageMask,
                                   VkAccessFlags2 dstAccessMask,
                                   VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                   uint32_t mipLevelCount = 1,
                                   uint32_t layerCount = 1) const;

    };
} // Rendering

#endif //MAXENGINE_VULKANCOMMANDBUFFERI_H
