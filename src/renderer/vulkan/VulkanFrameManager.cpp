//
// Created by zykov on 6/24/2026.
//

#include "VulkanFrameManager.h"

namespace Rendering {
    VulkanFrameManager::~VulkanFrameManager() {
    }

    VulkanFrameManager::VulkanFrameManager(ILogicalDevice &device, VulkanSwapChain &swapchain, const uint32_t frameCount)
        : device(static_cast<VulkanLogicalDevice *>(&device)),
          swapchain(&swapchain) {
        frames.reserve(frameCount);

        for (uint32_t i = 0; i < frameCount; ++i) {
            CreateFrame();
        }
    }

    void VulkanFrameManager::CreateFrame() {
        VulkanFrame frame;
        frame.ImageAvailableSemaphoreVk = device->CreateSemaphor();
        frame.RenderFinishedSemaphoreVk = device->CreateSemaphor();
        frame.InFlightVk = device->CreateFence();

        frames.push_back(frame);
    }

    VulkanFrame &VulkanFrameManager::AcquireFrame() {

        VulkanFrame &frame = frames[currentFrame];

        const VkDevice vkDevice = device->GetDevice();

        if (const VkResult result = vkWaitForFences(vkDevice, 1, &frame.InFlightVk, VK_TRUE, UINT64_MAX);
            result != VK_SUCCESS) {
            throw std::runtime_error("failed to wait for fences!");
        }

        if (const VkResult result = vkResetFences(vkDevice, 1, &frame.InFlightVk);
            result != VK_SUCCESS) {
            throw std::runtime_error("failed to reset a fences!");
            }

        uint32_t imageIndex = 0;

        if (const VkResult result = vkAcquireNextImageKHR(vkDevice, swapchain->GetSwapChain(), UINT64_MAX,
                                                          frame.ImageAvailableSemaphoreVk, VK_NULL_HANDLE, &imageIndex);
            result != VK_SUCCESS) {
            throw std::runtime_error("failed to acquire next image!");
        };

        frame.ColorTexture = swapchain->GetSwapChainImageView(imageIndex);
        frame.imageIndex = imageIndex;

        currentFrame = (currentFrame + 1) % frames.size();

        return frame;
    }


    uint32_t VulkanFrameManager::GetCurrentIndexFrame() const {
        return currentFrame;
    }

    VulkanFrame& VulkanFrameManager::GetCurrentFrame() {
        return frames[currentFrame];
    }
} // Rendering
