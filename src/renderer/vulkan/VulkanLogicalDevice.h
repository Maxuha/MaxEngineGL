//
// Created by zykov on 6/17/2026.
//

#ifndef MAXENGINE_VULKANLOGICALDEVICE_H
#define MAXENGINE_VULKANLOGICALDEVICE_H
#include <vulkan/vulkan_core.h>
#include "VulkanPhysicalDevice.h"
#include "VulkanSemaphore.h"
#include "../ILogicalDevice.h"

namespace Rendering {
    class ISemaphore;

    class VulkanLogicalDevice : public ILogicalDevice {
    public:
        explicit VulkanLogicalDevice(VulkanPhysicalDevice& physicalDevice);

        ~VulkanLogicalDevice() override;

        VulkanPhysicalDevice& GetPhysicalDevice() const;

        VkDevice GetDevice() const;

        VkQueue GetGraphicsQueue() const;
        VkQueue GetPresentQueue() const;
        VkQueue GetTransferQueue() const;
        VkQueue GetComputeQueue() const;

        Result AcquireNextImage(const ISwapchain &swapchain, const ISemaphore &semaphore, uint32_t& imageIndex) ;

        Result WaitIdle(const VkFence &fence);

        VkSemaphore CreateSemaphor();

        VkFence CreateFence() const;

        VkMemoryRequirements GetMemoryRequirements(const VkImage &image) const;

    private:
        VulkanPhysicalDevice& physicalDevice;

        VkDevice device;

        VkQueue graphicsQueue;
        VkQueue presentQueue;
        VkQueue computeQueue;
        VkQueue transferQueue;

        Result ToResult(VkResult result);
    };
} // Rendering

#endif //MAXENGINE_VULKANLOGICALDEVICE_H
