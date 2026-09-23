//
// Created by zykov on 6/17/2026.
//

#include "VulkanPhysicalDevice.h"
#include "VulkanInstance.h"

namespace Rendering {
    VulkanPhysicalDevice::VulkanPhysicalDevice(const VulkanInstance& instance, const VulkanSurface& surface) {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance.GetInstance(), &deviceCount, nullptr);

        if (deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance.GetInstance(), &deviceCount, devices.data());

        for (const auto &device: devices) {
            indices = VulkanQueueFamily::FindQueueFamilies(device, surface.GetSurface());
            if (IsDeviceSuitable(device, surface)) {

                physicalDevice = device;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    VulkanPhysicalDevice::~VulkanPhysicalDevice() {
    }

    VkPhysicalDevice VulkanPhysicalDevice::GetPhysicalDevice() const {
        return physicalDevice;
    }

    QueueFamilyIndices VulkanPhysicalDevice::GetQueueFamilyIndices() {
        return indices;
    }

    SwapChainSupportDetails VulkanPhysicalDevice::QuerySwapChainSupport(const VulkanSurface& surface) const {
       return QuerySwapChainSupport(surface, physicalDevice);
    }

    uint32_t VulkanPhysicalDevice::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    VkFormat VulkanPhysicalDevice::FindDepthFormat() const {
        return FindSupportedFormat({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                                   VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }

    VkFormat VulkanPhysicalDevice::FindSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling,
                                                       VkFormatFeatureFlags features) const {
        for (const auto &format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                return format;
            }

            if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }

        throw std::runtime_error("failed to find supported format!");
    }

    bool VulkanPhysicalDevice::HasStencilComponent(VkFormat format) const {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    }

    SwapChainSupportDetails VulkanPhysicalDevice::QuerySwapChainSupport(const VulkanSurface &surface,
                                                                        const VkPhysicalDevice device) const {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface.GetSurface(), &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.GetSurface(), &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.GetSurface(), &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.GetSurface(), &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.GetSurface(), &presentModeCount, details.presentModes.data());
        }
        return details;
    }

    bool VulkanPhysicalDevice::IsDeviceSuitable(const VkPhysicalDevice device, const VulkanSurface& surface) {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        bool extensionsSupported = true;
        bool swapChainAdequate = false;
        if (extensionsSupported) {
            const SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(surface, device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
               deviceFeatures.geometryShader && indices.IsComplete() && swapChainAdequate;
    }

    int VulkanPhysicalDevice::RateDeviceSuitability(VkPhysicalDevice device) {
        return -1;
    }
} // Rendering
