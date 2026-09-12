//
// Created by zykov on 6/17/2026.
//

#ifndef MAXENGINE_VULKANQUEUEFAMILY_H
#define MAXENGINE_VULKANQUEUEFAMILY_H
#include <vulkan/vulkan_core.h>
#include <optional>

namespace Rendering {
    class VulkanPhysicalDevice;
    class VulkanLogicalDevice;

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        std::optional<uint32_t> computeFamily;
        std::optional<uint32_t> transferFamily;

        bool IsComplete() const {
            return graphicsFamily.has_value() && presentFamily.has_value() && computeFamily.has_value() && transferFamily.has_value();
        }
    };

    class VulkanQueueFamily {
    public:
        VulkanQueueFamily() = default;
        ~VulkanQueueFamily() = default;

        static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface = VK_NULL_HANDLE);
    };
} // Rendering

#endif //MAXENGINE_VULKANQUEUEFAMILY_H
