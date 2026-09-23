//
// Created by zykov on 6/17/2026.
//

#include "VulkanQueueFamily.h"

namespace Rendering {
QueueFamilyIndices VulkanQueueFamily::FindQueueFamilies(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    if (queueFamilyCount == 0) return indices;

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    for (uint32_t i = 0; i < queueFamilyCount; ++i) {
        const auto& queueFamily = queueFamilies[i];

        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            if (!indices.graphicsFamily.has_value()) {
                indices.graphicsFamily = i;
            }
        }

        if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
            if (!indices.computeFamily.has_value() || !(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
                indices.computeFamily = i;
            }
        }

        if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) {
            if (!indices.transferFamily.has_value() ||
               (!(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) && !(queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT))) {
                indices.transferFamily = i;
            }
        }

        if (surface != VK_NULL_HANDLE) {
            VkBool32 presentSupport = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

            if (presentSupport) {
                if (!indices.presentFamily.has_value() || (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
                    indices.presentFamily = i;
                }
            }
        }
    }

    if (indices.graphicsFamily.has_value()) {
        if (!indices.computeFamily.has_value())   indices.computeFamily = indices.graphicsFamily;
        if (!indices.transferFamily.has_value())  indices.transferFamily = indices.graphicsFamily;
        if (surface == VK_NULL_HANDLE)            indices.presentFamily = indices.graphicsFamily;
    }

    return indices;
}
} // Rendering
