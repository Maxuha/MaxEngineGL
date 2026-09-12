//
// Created by zykov on 6/17/2026.
//

#include "VulkanLogicalDevice.h"

#include <set>

#include "VulkanFence.h"
#include "VulkanQueueFamily.h"
#include "VulkanSemaphore.h"
#include "VulkanSwapChain.h"

namespace Rendering {
    VulkanLogicalDevice::VulkanLogicalDevice(VulkanPhysicalDevice& physicalDevice): physicalDevice(physicalDevice) {
        QueueFamilyIndices indices = physicalDevice.GetQueueFamilyIndices();

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value(), indices.computeFamily.value(), indices.transferFamily.value()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily: uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice.GetPhysicalDevice(), &deviceProperties);

        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(physicalDevice.GetPhysicalDevice(), &deviceFeatures);

        if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ||
            !deviceFeatures.geometryShader) {
            throw std::runtime_error("device is not suitable");
        }

        std::vector deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        VkPhysicalDeviceVulkan13Features features13{};
        features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        features13.dynamicRendering = VK_TRUE;
        features13.synchronization2 = VK_TRUE;

        VkDeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pNext = &features13;
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
        deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
        deviceCreateInfo.enabledLayerCount = 0;

        if (const VkResult result = vkCreateDevice(physicalDevice.GetPhysicalDevice(), &deviceCreateInfo, nullptr, &device);
            result != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!" + std::to_string(result));
        }

        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
        vkGetDeviceQueue(device, indices.transferFamily.value(), 0, &transferQueue);
        vkGetDeviceQueue(device, indices.computeFamily.value(), 0, &computeQueue);
    }

    VulkanLogicalDevice::~VulkanLogicalDevice() {
    }

    VulkanPhysicalDevice & VulkanLogicalDevice::GetPhysicalDevice() const {
        return physicalDevice;
    }

    VkDevice VulkanLogicalDevice::GetDevice() const {
        return device;
    }

    VkQueue VulkanLogicalDevice::GetGraphicsQueue() const {
        return graphicsQueue;
    }

    VkQueue VulkanLogicalDevice::GetPresentQueue() const {
        return presentQueue;
    }

    VkQueue VulkanLogicalDevice::GetTransferQueue() const {
        return transferQueue;
    }

    VkQueue VulkanLogicalDevice::GetComputeQueue() const {
        return computeQueue;
    }

    Result VulkanLogicalDevice::AcquireNextImage(const ISwapchain &swapchain, const ISemaphore &semaphore,
                                                 uint32_t& imageIndex) {
        // return ToResult(vkAcquireNextImageKHR(device, static_cast<const VulkanSwapChain &>(swapchain).GetSwapChain(), UINT64_MAX,
        //                        static_cast<const VulkanSemaphore &>(semaphore).GetSemaphore(), VK_NULL_HANDLE, &imageIndex));
        return Result{};
    }

    Result VulkanLogicalDevice::WaitIdle(const VkFence &fence) {
        const VkResult res = vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);

        Result vkWaitForFencesResult = ToResult(res);

        if (!vkWaitForFencesResult.Success) {
            return vkWaitForFencesResult;
        }

        const VkResult res2 = vkResetFences(device, 1, &fence);

        Result vkResetFencesResult = ToResult(res2);

        auto result = Result {
            .Success = vkWaitForFencesResult.Success && vkResetFencesResult.Success
        };

        return result;

    }
    //
    // Result VulkanLogicalDevice::WaitIdle(const IFence &fence) {
    //     const VkFence vulkanFence = static_cast<const VulkanFence &>(fence).GetFence();
    //
    //     Result vkWaitForFencesResult = ToResult(vkWaitForFences(device, 1, &vulkanFence, VK_TRUE, UINT64_MAX));
    //     Result vkResetFencesResult = ToResult(vkResetFences(device, 1, &vulkanFence));
    //
    //     auto result = Result {
    //         .Success = vkWaitForFencesResult.Success && vkResetFencesResult.Success
    //     };
    //
    //     return result;
    // }

    VkSemaphore VulkanLogicalDevice::CreateSemaphor() {
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkSemaphore semaphore = nullptr;

        const VkResult result = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphore);

        return semaphore;
    }

    VkFence VulkanLogicalDevice::CreateFence() const {
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        VkFence fence = nullptr;

        const VkResult result = vkCreateFence(device, &fenceInfo, nullptr, &fence);

        return fence;
    }

    VkMemoryRequirements VulkanLogicalDevice::GetMemoryRequirements(const VkImage &image) const {
        VkMemoryRequirements memoryRequirements;
        vkGetImageMemoryRequirements(device, image, &memoryRequirements);
        return memoryRequirements;
    }


    Result VulkanLogicalDevice::ToResult(const VkResult result) {
        return Result(result == VK_SUCCESS);
    }


} // Rendering
