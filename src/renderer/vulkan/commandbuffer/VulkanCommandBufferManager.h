//
// Created by zykov on 6/19/2026.
//

#ifndef MAXENGINE_VULKANCOMMANDBUFFER_H
#define MAXENGINE_VULKANCOMMANDBUFFER_H
#include <vulkan/vulkan_core.h>

#include "../../ICommandBufferManager.h"
#include "VulkanCommandBufferi.h"

namespace Rendering {
    enum class CommandPoolType;
    class VulkanLogicalDevice;

    class VulkanCommandBufferManager : public ICommandBufferManager {
        public:
        explicit VulkanCommandBufferManager(VulkanLogicalDevice& logicalDevice);
        ~VulkanCommandBufferManager() override;

        VkCommandBuffer GetCommandBuffer(CommandBufferHandle handle) const;

        VkCommandBuffer AllocateCommandBuffer(CommandPoolType type, VkCommandBufferLevel level);

    private:
        VulkanLogicalDevice& logicalDevice;

        std::vector<VulkanCommandBufferi> commandBuffers;
        std::vector<VkCommandBuffer> graphicsCommandBuffers;
        std::vector<VkCommandBuffer> transferCommandBuffers;

        VkCommandPool AllocateCommandPool(std::optional<uint32_t> queueFamily) const;

        VkCommandPool graphicsCommandPool;
        VkCommandPool transferCommandPool;
    };
} // Rendering

#endif //MAXENGINE_VULKANCOMMANDBUFFER_H
