//
// Created by zykov on 6/19/2026.
//

#include "VulkanCommandBufferManager.h"

#include "../../ICommandPoolManager.h"
#include "../VulkanLogicalDevice.h"


namespace Rendering {
    VulkanCommandBufferManager::VulkanCommandBufferManager(VulkanLogicalDevice &logicalDevice) : logicalDevice(logicalDevice) {
        const std::optional<uint32_t> graphicsFamily = logicalDevice.GetPhysicalDevice().GetQueueFamilyIndices().graphicsFamily;
        const std::optional<uint32_t> transferFamily = logicalDevice.GetPhysicalDevice().GetQueueFamilyIndices().transferFamily;
        graphicsCommandPool = AllocateCommandPool(graphicsFamily);
        transferCommandPool = AllocateCommandPool(transferFamily);
    }

    VulkanCommandBufferManager::~VulkanCommandBufferManager() {
        // for (auto commandBuffer : graphicsCommandBuffers) {
        //     vkFreeCommandBuffers(logicalDevice.GetDevice(), graphicsCommandPool, 1, &commandBuffer);
        // }
        vkDestroyCommandPool(logicalDevice.GetDevice(), graphicsCommandPool, nullptr);
    }

    VkCommandBuffer VulkanCommandBufferManager::GetCommandBuffer(const CommandBufferHandle handle) const {
        return commandBuffers[handle.Id].GetCommandBuffer();
    }

    VulkanCommandBufferi VulkanCommandBufferManager::AllocateCommandBuffer(const CommandPoolType type, const VkCommandBufferLevel level) const {
        VkCommandBuffer commandBuffer;
        VkCommandPool vkCommandPool = nullptr;

        switch (type) {
            case CommandPoolType::Graphics:
                vkCommandPool = graphicsCommandPool;
                break;
            case CommandPoolType::Compute:
                break;
            case CommandPoolType::Transfer:
                vkCommandPool = transferCommandPool;
                break;
            default:
                throw std::runtime_error("invalid command pool type!");
        }

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = vkCommandPool;
        allocInfo.level = level;
        allocInfo.commandBufferCount = 1;

        if (const VkResult result = vkAllocateCommandBuffers(logicalDevice.GetDevice(), &allocInfo, &commandBuffer); result != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        return VulkanCommandBufferi(commandBuffer, type);
    }

    VkCommandPool VulkanCommandBufferManager::AllocateCommandPool(const std::optional<uint32_t> queueFamily) const {
        VkCommandPool commandPool;

        const uint32_t queueFamilyIndex = queueFamily.value();

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndex;

        if (const VkResult result = vkCreateCommandPool(logicalDevice.GetDevice(), &poolInfo, nullptr, &commandPool);
            result != VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool!");
            }

        return commandPool;
    }

    void VulkanCommandBufferManager::SendCommandBuffer(const VulkanCommandBufferi &commandBuffer, CommandPoolType type,
        const VkSemaphore wait, const VkSemaphore signal, const VkFence fence) const {
        const VkCommandBuffer vkCommandBuffer = commandBuffer.GetCommandBuffer();

        const VkSemaphore waitSemaphores[] = {wait};
        const VkSemaphore signalSemaphores[] = {signal};
        constexpr VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &vkCommandBuffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (const VkResult result = vkQueueSubmit(logicalDevice.GetGraphicsQueue(), 1, &submitInfo, fence);
            result != VK_SUCCESS) {
            throw std::runtime_error("failed to submit command buffer!: " + std::to_string(result));
            }
    }

    void VulkanCommandBufferManager::SendCommandBufferAndWait(const VulkanCommandBufferi &commandBuffer, CommandPoolType type) const {
        const VkCommandBuffer vkCommandBuffer = commandBuffer.GetCommandBuffer();

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &vkCommandBuffer;

        if (vkQueueSubmit(logicalDevice.GetTransferQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!:");
        }

        vkQueueWaitIdle(logicalDevice.GetTransferQueue());
    }
} // Rendering