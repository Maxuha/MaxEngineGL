//
// Created by zykov on 6/17/2026.
//

#ifndef MAXENGINE_VULKANPHYSICALDEVICE_H
#define MAXENGINE_VULKANPHYSICALDEVICE_H
#include <vulkan/vulkan_core.h>

#include "VulkanQueueFamily.h"
#include "VulkanSurface.h"
#include "../IPhysicalDevice.h"

namespace Rendering {
    class VulkanPhysicalDevice : public IPhysicalDevice {
    public:
        explicit VulkanPhysicalDevice(const VulkanInstance& instance, const VulkanSurface& surface);

        ~VulkanPhysicalDevice() override;

        VkPhysicalDevice GetPhysicalDevice() const;

        QueueFamilyIndices GetQueueFamilyIndices() override;

        SwapChainSupportDetails QuerySwapChainSupport(const VulkanSurface& surface) const;

        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        VkFormat FindDepthFormat() const;

        VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

        bool HasStencilComponent(VkFormat format) const;

    private:
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        QueueFamilyIndices indices;

        SwapChainSupportDetails QuerySwapChainSupport(const VulkanSurface& surface, VkPhysicalDevice device) const;
        bool IsDeviceSuitable(VkPhysicalDevice device, const VulkanSurface& surface);
        int RateDeviceSuitability(VkPhysicalDevice device);

    };
} // Rendering

#endif //MAXENGINE_VULKANPHYSICALDEVICE_H
