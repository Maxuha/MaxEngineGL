//
// Created by zykov on 8/28/2026.
//

#include "VulkanStagingBufferPool.h"

namespace Rendering {
    VulkanStagingBufferPool::VulkanStagingBufferPool(const VkDevice device, const VkPhysicalDevice physicalDevice,
                                                     const VkDeviceSize poolSizeInBytes) : device(device),
        physicalDevice(physicalDevice), poolSizeInBytes(poolSizeInBytes) {
        VkBufferCreateInfo bufferInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        bufferInfo.size = poolSizeInBytes;
        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device, &bufferInfo, nullptr, &stagingBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create staging buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, stagingBuffer, &memRequirements);

        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        uint32_t memoryTypeIndex = UINT32_MAX;
        constexpr VkMemoryPropertyFlags properties =
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((memRequirements.memoryTypeBits & (1 << i)) &&
                (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                memoryTypeIndex = i;
                break;
            }
        }

        VkMemoryAllocateInfo allocInfo{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = memoryTypeIndex;

        if (vkAllocateMemory(device, &allocInfo, nullptr, &stagingBufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate staging buffer memory!");
        }

        vkBindBufferMemory(device, stagingBuffer, stagingBufferMemory, 0);

        vkMapMemory(device, stagingBufferMemory, 0, poolSizeInBytes, 0, &cpuMappedPointer);
    }

    VulkanStagingBufferPool::~VulkanStagingBufferPool() {
        vkUnmapMemory(device, stagingBufferMemory);
        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);
    }

    StagingAllocation VulkanStagingBufferPool::Allocate(const VkDeviceSize size, const VkDeviceSize alignment) {
        const VkDeviceSize alignedOffset = (offset + alignment - 1) & ~(alignment - 1);
        if (alignedOffset + size > poolSizeInBytes) {
            throw std::runtime_error("Failed to allocate staging buffer!");
        }

        offset = alignedOffset + size;

        StagingAllocation allocation;
        allocation.vkBuffer = stagingBuffer;
        allocation.offset = alignedOffset;
        allocation.size = size;
        allocation.cpuMappedPointer = static_cast<uint8_t *>(cpuMappedPointer) + alignedOffset;
        return allocation;
    }

    void VulkanStagingBufferPool::Reset() {
        offset = 0;
    }
} // Rendering
