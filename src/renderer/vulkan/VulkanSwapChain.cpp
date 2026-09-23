//
// Created by zykov on 6/17/2026.
//

#include "VulkanSwapChain.h"

#include "VulkanFrameManager.h"
#include "VulkanLogicalDevice.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanQueueFamily.h"
#include "texture/VulkanTextureManager.h"
#include "../../renderer/gl/model/TextureData.h"

namespace Rendering {
    struct VulkanFrame;

    VulkanSwapChain::VulkanSwapChain(
        VulkanPhysicalDevice &physicalDevice,
        ILogicalDevice &logicalDevice,
        const VulkanSurface &surface,
        IWindow &window,
        VulkanFrameBufferManager &frameBufferManager,
        VulkanRenderPassManager& renderPassManager
    ) : frameBufferManager(&frameBufferManager), logicalDevice(static_cast<VulkanLogicalDevice *>(&logicalDevice)), renderPassManager(&renderPassManager) {
        SwapChainSupportDetails swapChainSupport = physicalDevice.QuerySwapChainSupport(surface);
        VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
        const VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities, &window);

        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.pNext = nullptr;
        createInfo.surface = surface.GetSurface();
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = swapChainImageFormat;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = physicalDevice.GetQueueFamilyIndices();
        const uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (const VkResult result = vkCreateSwapchainKHR(this->logicalDevice->GetDevice(), &createInfo, nullptr, &swapChain);
            result != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }
    }

    VulkanSwapChain::~VulkanSwapChain() {
        // vkDestroySwapchainKHR(logicalDevice->GetDevice(), swapChain, nullptr);
        // for (const auto imageView : swapChainImageViews) {
        //     vkDestroyImageView(logicalDevice->GentDevice(), imageView, nullptr);
        // }
    }

    uint32_t VulkanSwapChain::GetImageCount() const {
        uint32_t imageCount;
        vkGetSwapchainImagesKHR(logicalDevice->GetDevice(), swapChain, &imageCount, nullptr);
        return imageCount;
    }

    VkImageView *VulkanSwapChain::GetSwapChainImageViews() {
        return swapChainImageViews.data();
    }

    VulkanTexture VulkanSwapChain::GetSwapChainImageView(const uint32_t index) const {
        return swapChainTexturesData[index];
    }

    VkImageView VulkanSwapChain::GetDepthImageView() const {
        return depthImageView;
    }

    VkExtent2D VulkanSwapChain::GetExtent() const {
        return swapChainExtent;
    }

    VkSwapchainKHR VulkanSwapChain::GetSwapChain() const {
        return swapChain;
    }

    VkFormat VulkanSwapChain::GetImageFormat() const  {
        return swapChainImageFormat;
    }

    uint32_t VulkanSwapChain::GetCurrentImageIndex() const {
        return imageIndex;
    }

    VkResult VulkanSwapChain::AcquireNextImage(const ISemaphore *semaphore, uint32_t& imageIndex) {
        const VkResult result = vkAcquireNextImageKHR(logicalDevice->GetDevice(), GetSwapChain(), UINT64_MAX,
                               static_cast<const VulkanSemaphore *>(semaphore)->GetSemaphore(), VK_NULL_HANDLE, &imageIndex);

        this->imageIndex = imageIndex;

        return result;
    }

    void VulkanSwapChain::CreateTextures() {
        uint32_t imageCount = GetImageCount();

        std::vector<VkImage> images;
        images.reserve(imageCount);

        vkGetSwapchainImagesKHR(this->logicalDevice->GetDevice(), swapChain, &imageCount, images.data());

        std::vector<VkImageView> image_views;
        image_views.reserve(imageCount);

        for (size_t i = 0; i < imageCount; i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = images[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = swapChainImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (const VkResult result = vkCreateImageView(logicalDevice->GetDevice(), &createInfo, nullptr,
                                                          &image_views[i]);
                result != VK_SUCCESS) {
                    throw std::runtime_error("failed to create image views!");
                }

            swapChainTexturesData.push_back(VulkanTexture { .Image = images[i], .ImageView = image_views[i] });

            // swapChainTextures.push_back(textureManager->CreateTexture(images[i], image_views[i], swapChainExtent.width, swapChainExtent.height));
        }
    }

    void VulkanSwapChain::CreateFrameBuffers() const {
        // for (size_t i = 0; i < GetImageCount(); i++) {
        //    // swapChainFramebuffers.push_back(frameBufferManager->CreateFramebuffer(RenderPassHandle {.Id = 0}, swapChainTextures[i]));
        // }
    }

    FrameBufferHandle VulkanSwapChain::GetCurrentFrameBuffer() const {
        return swapChainFramebuffers[0];
    }

    void VulkanSwapChain::Present(const VulkanFrame* frame) const {
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &frame->RenderFinishedSemaphoreVk;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swapChain;
        presentInfo.pImageIndices = &frame->imageIndex;
        presentInfo.pResults = nullptr;

        vkQueuePresentKHR(logicalDevice->GetPresentQueue(), &presentInfo);
    }

    VkSurfaceFormatKHR VulkanSwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
        for (const auto &availableFormat: availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace ==
                VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR VulkanSwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) {
        for (const auto &availablePresentMode: availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VulkanSwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, IWindow *window) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        }

        Rect viewport = window->GetCurrentSize();

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(viewport.width),
            static_cast<uint32_t>(viewport.height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                                        capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                                         capabilities.maxImageExtent.height);

        return actualExtent;
    }

    void VulkanSwapChain::CreateImageViews() {
        swapChainImageViews.resize(swapChainImages.size());

        for (size_t i = 0; i < swapChainImages.size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = swapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = swapChainImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;

            if (const VkResult result = vkCreateImageView(logicalDevice->GetDevice(), &createInfo, nullptr,
                                                          &swapChainImageViews[i]);
                result != VK_SUCCESS) {
                throw std::runtime_error("failed to create image views!");
            }
        }
    }
} // Rendering
