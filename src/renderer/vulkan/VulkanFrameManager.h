//
// Created by zykov on 6/24/2026.
//

#ifndef MAXENGINE_FRAMEMANAGER_H
#define MAXENGINE_FRAMEMANAGER_H
#include "VulkanSwapChain.h"
#include "../src/renderer/vulkan/VulkanRenderPassManager.h"

namespace Rendering {
    class ILogicalDevice;
    class VulkanSwapChain;

    struct VulkanFrame {
        mutable VulkanTexture ColorTexture;
        mutable uint32_t imageIndex;
        VkSemaphore ImageAvailableSemaphoreVk;
        VkSemaphore RenderFinishedSemaphoreVk;
        VkFence InFlightVk;

        // VulkanFrame(const VulkanFrame&) = delete;
        // VulkanFrame& operator=(const VulkanFrame&) = delete;
        //
        // VulkanFrame(VulkanFrame&&) noexcept = default;
        // VulkanFrame& operator=(VulkanFrame&&) noexcept = default;

    };

    class VulkanFrameManager {
    public:
        ~VulkanFrameManager();

        VulkanFrameManager(ILogicalDevice &device, VulkanSwapChain &swapchain, uint32_t frameCount);

        void CreateFrame();

        VulkanFrame& AcquireFrame();

        uint32_t GetCurrentIndexFrame() const;

        VulkanFrame& GetCurrentFrame();

    private:
        VulkanLogicalDevice *device;
        VulkanSwapChain *swapchain;

        std::vector<VulkanFrame> frames;
        uint32_t currentFrame = 0;
    };
} // Rendering

#endif //MAXENGINE_FRAMEMANAGER_H
