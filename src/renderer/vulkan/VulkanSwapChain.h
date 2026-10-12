//
// Created by zykov on 6/17/2026.
//

#ifndef MAXENGINE_VULKANSWAPCHAIN_H
#define MAXENGINE_VULKANSWAPCHAIN_H
#include "../ISwapchain.h"
#include <../src/renderer/IRenderDevice.h>
#include "../src/math/Rect.h"
#include "texture/VulkanTextureManager.h"

namespace Rendering {
    class VulkanLogicalDevice;
    class VulkanPhysicalDevice;
    class VulkanSurface;
    class VulkanTextureManager;
    class ISemaphore;
    struct VulkanFrame;

    class VulkanSwapChain {
    public:
        VulkanSwapChain(
            VulkanPhysicalDevice &physicalDevice,
            ILogicalDevice& logicalDevice,
            const VulkanSurface &surface,
            IWindow& window
            );

        ~VulkanSwapChain();

        uint32_t GetImageCount() const;

        VulkanTexture GetSwapChainImageView(uint32_t index) const;

        VkSwapchainKHR GetSwapChain() const;

        VkFormat GetImageFormat() const;

        uint32_t GetCurrentImageIndex() const;

        VkResult AcquireNextImage(VkSemaphore semaphore, uint32_t& imageIndex);

        void Present(VkSemaphore wait, uint32_t imageIndex) const;

    private:
        VulkanLogicalDevice* logicalDevice;
        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages;
        VkFormat imageFormat;
        VkExtent2D extent;
        std::vector<VulkanTexture> ColorTextures;
        uint32_t imageIndex = 0;

        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

        VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, IWindow *window);

        void CreateTextures();

    };
} // Rendering

#endif //MAXENGINE_VULKANSWAPCHAIN_H
