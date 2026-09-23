//
// Created by zykov on 6/24/2026.
//

#ifndef MAXENGINE_FRAMEMANAGER_H
#define MAXENGINE_FRAMEMANAGER_H
#include "VulkanSwapChain.h"
#include "../IFrameManager.h"
#include "../src/renderer/vulkan/framebuffer/VulkanFrameBufferManager.h"
#include "../src/renderer/vulkan/VulkanRenderPassManager.h"
#include "../src/renderer/vulkan/VulkanSwapChain.h"
#include "../ICommandPoolManager.h"
#include "../src/renderer/RenderContext.h"

namespace Rendering {
    class ISemaphore;
    class IFence;
    class ICommandBuffer;
    class ILogicalDevice;
    class ISwapchain;
    class VulkanSwapChain;
    class IFrameBufferManager;
    struct FrameBufferHandle;
    struct RenderContext;

    struct VulkanFrame {
        FrameBufferHandle FrameBuffer;
        mutable VulkanTexture ColorTexture;
        mutable VkImageView DepthImageView;
        mutable uint32_t imageIndex;
        VkSemaphore ImageAvailableSemaphoreVk;
        VkSemaphore RenderFinishedSemaphoreVk;
        VkFence InFlightVk;
        VkCommandBuffer commandBuffer;
        VkCommandBuffer secondaryCommandBuffer;
        Rect Viewport;

        // VulkanFrame(const VulkanFrame&) = delete;
        // VulkanFrame& operator=(const VulkanFrame&) = delete;
        //
        // VulkanFrame(VulkanFrame&&) noexcept = default;
        // VulkanFrame& operator=(VulkanFrame&&) noexcept = default;

    };

    class VulkanFrameManager {
    public:
        ~VulkanFrameManager();

        VulkanFrameManager(ILogicalDevice &device, VulkanSwapChain &swapchain,
                     IFrameBufferManager &frameBufferManager, ICommandBufferManager &commandBufferManager);

        void CreateFrames(RenderPassHandle renderPass, TextureHandle texture);

        VulkanFrame& AcquireFrame();

        void SendFrameToGPU(const VulkanFrame& frame) const;

        void Present(const VulkanFrame& frame) const;

        CommandBufferHandle GetCommandBufferForCurrentFrame() const;

        FrameBufferHandle GetCurrentFrame() const;

        VulkanFrame GetVulkanFrame() const;

        uint32_t GetCurrentIndexFrame() const;

    private:
        VulkanLogicalDevice *device;
        CommandBuffer *commandBuffer;
        VulkanSwapChain *swapchain;
        IFrameBufferManager *frameBufferManager;
        ICommandBufferManager *commandBufferManager;
        CommandPoolHandle commandPool;

        std::vector<VulkanFrame> frames;
        uint32_t currentFrame = 0;

        ImageFormat MapVkFormat(const VkFormat format) {
            switch (format) {
                case VK_FORMAT_R8G8B8A8_UNORM: {
                    return ImageFormat::BGRA8_Srgb;
                }
                case VK_FORMAT_R8G8B8A8_SRGB: {
                    return ImageFormat::RGBA8_Srgb;
                }
                case VK_FORMAT_B8G8R8A8_SRGB: {
                    return ImageFormat::BGRA8_Srgb;
                }
                default: {
                    return ImageFormat::Undefined;
                }
            }
        }
    };
} // Rendering

#endif //MAXENGINE_FRAMEMANAGER_H
