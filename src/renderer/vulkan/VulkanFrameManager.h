//
// Created by zykov on 6/24/2026.
//

#ifndef MAXENGINE_FRAMEMANAGER_H
#define MAXENGINE_FRAMEMANAGER_H
#include "VulkanSwapChain.h"

namespace Rendering {
    class ILogicalDevice;
    class VulkanSwapChain;

    struct VulkanFrame {
        mutable VulkanTexture ColorTexture;
        mutable uint32_t imageIndex;
        VkSemaphore ImageAvailableSemaphoreVk;
        VkSemaphore RenderFinishedSemaphoreVk;
        VkFence InFlightVk;

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
