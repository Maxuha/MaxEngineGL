//
// Created by zykov on 8/28/2026.
//

#ifndef MAXENGINE_VULKANSTAGINGBUFFERPOOL_H
#define MAXENGINE_VULKANSTAGINGBUFFERPOOL_H
#include <vulkan/vulkan_core.h>

namespace Rendering {
    struct StagingAllocation {
        VkBuffer vkBuffer       = VK_NULL_HANDLE;
        VkDeviceSize offset     = 0;
        VkDeviceSize size       = 0;
        void* cpuMappedPointer  = nullptr;

        bool IsValid() const { return vkBuffer != VK_NULL_HANDLE; }
    };

    class VulkanStagingBufferPool {
    public:
        VulkanStagingBufferPool(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize poolSizeInBytes);
        ~VulkanStagingBufferPool();

        StagingAllocation Allocate(VkDeviceSize size, VkDeviceSize alignment = 4);

        void Reset();

    private:
        VkDevice device;
        VkPhysicalDevice physicalDevice;
        VkDeviceSize poolSizeInBytes;
        VkDeviceSize offset = 0;
        VkBuffer stagingBuffer = VK_NULL_HANDLE;
        VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;
        void* cpuMappedPointer = nullptr;
    };
} // Rendering

#endif //MAXENGINE_VULKANSTAGINGBUFFERPOOL_H
