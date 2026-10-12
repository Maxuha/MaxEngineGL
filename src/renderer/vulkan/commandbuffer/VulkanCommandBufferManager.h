//
// Created by zykov on 6/19/2026.
//

#ifndef MAXENGINE_VULKANCOMMANDBUFFER_H
#define MAXENGINE_VULKANCOMMANDBUFFER_H
#include <vulkan/vulkan_core.h>

#include "../../ICommandBufferManager.h"
#include "VulkanCommandBufferi.h"
#include "../../gl/CommandBuffer.h"

namespace Rendering {
    enum class CommandPoolType;
    class VulkanLogicalDevice;

    class VulkanCommandBufferManager : public ICommandBufferManager {
        public:
        explicit VulkanCommandBufferManager(VulkanLogicalDevice& logicalDevice);
        ~VulkanCommandBufferManager() override;

        VkCommandBuffer GetCommandBuffer(CommandBufferHandle handle) const;

        VulkanCommandBufferi AllocateCommandBuffer(CommandPoolType type, VkCommandBufferLevel level) const;

        void SendCommandBuffer(const VulkanCommandBufferi &commandBuffer, CommandPoolType type, VkSemaphore wait, VkSemaphore signal, VkFence fence) const;

        void SendCommandBufferAndWait(const VulkanCommandBufferi &commandBuffer, CommandPoolType type) const;

    private:
        VulkanLogicalDevice& logicalDevice;

        std::vector<VulkanCommandBufferi> commandBuffers;

        VkCommandPool AllocateCommandPool(std::optional<uint32_t> queueFamily) const;

        VkCommandPool graphicsCommandPool;
        VkCommandPool transferCommandPool;
    };
} // Rendering

#endif //MAXENGINE_VULKANCOMMANDBUFFER_H
