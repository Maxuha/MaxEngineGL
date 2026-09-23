//
// Created by zykov on 6/17/2026.
//

#ifndef MAXENGINE_VULKANSWAPCHAIN_H
#define MAXENGINE_VULKANSWAPCHAIN_H
#include "../ISwapchain.h"
#include "../../renderer/gl/model/TextureData.h"
#include "framebuffer/VulkanFrameBufferManager.h"
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
            IWindow& window,
            VulkanFrameBufferManager& frameBufferManager,
            VulkanRenderPassManager& renderPassManager
            );

        ~VulkanSwapChain();

        uint32_t GetImageCount() const;

        VkImageView* GetSwapChainImageViews();

        VulkanTexture GetSwapChainImageView(uint32_t index) const;

        VkImageView GetDepthImageView() const;

        Rect GetViewport() {
            return Rect { 0.0f, 0.0f, static_cast<float>(swapChainExtent.width), static_cast<float>(swapChainExtent.height) };
        }

        VkExtent2D GetExtent() const;

        VkSwapchainKHR GetSwapChain() const;

        VkFormat GetImageFormat() const;

        uint32_t GetCurrentImageIndex() const;

        TextureHandle GetTexture(const uint32_t index) const {
            return swapChainTextures[index];
        }

        VkResult AcquireNextImage(const ISemaphore* semaphore, uint32_t& imageIndex);

        void CreateTextures();

        void CreateFrameBuffers() const;

        FrameBufferHandle GetCurrentFrameBuffer() const;

        void Present(const VulkanFrame* frame) const;

    private:
        VulkanFrameBufferManager* frameBufferManager;
        VulkanLogicalDevice* logicalDevice;
        VulkanRenderPassManager* renderPassManager;
        VkSwapchainKHR swapChain;
        std::vector<VkImage> swapChainImages;
        std::vector<VkImageView> swapChainImageViews;
        VkFormat swapChainImageFormat;
        VkExtent2D swapChainExtent;
        std::vector<TextureHandle> swapChainTextures;
        std::vector<FrameBufferHandle> swapChainFramebuffers;
        std::vector<VulkanTexture> swapChainTexturesData;
        uint32_t imageIndex = 0;

        VkImage depthImage;
        VkDeviceMemory depthImageMemory;
        VkImageView depthImageView;

        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

        VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, IWindow *window);

        void CreateImageViews();
    };
} // Rendering

#endif //MAXENGINE_VULKANSWAPCHAIN_H
