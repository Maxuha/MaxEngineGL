//
// Created by zykov on 6/19/2026.
//

#include "VulkanFrameBufferManager.h"
#include "../texture/VulkanTextureManager.h"
#include "../VulkanRenderPassManager.h"
#include "../VulkanLogicalDevice.h"

namespace Rendering {
    VulkanFrameBufferManager::VulkanFrameBufferManager(
        VulkanLogicalDevice &device,
        VulkanRenderPassManager &renderPassManager,
        VulkanTextureManager &textureManager
    ) : device(device), renderPassManager(renderPassManager), textureManager(textureManager) {
    }

    FrameBufferHandle VulkanFrameBufferManager::CreateFramebuffer(
        const RenderPassHandle renderPass,
        const TextureHandle texture
        ) {

        const VulkanRenderPass vkRenderPass = renderPassManager.GetRenderPass(renderPass);

        VkFramebuffer framebuffer;

        const auto& textureData = textureManager.GetTexture(texture);

        const VkImageView attachment = textureData.ImageView;
        const uint32_t width = textureData.Width;
        const uint32_t height = textureData.Height;

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = vkRenderPass.RenderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = &attachment;
        framebufferInfo.width = width;
        framebufferInfo.height = height;
        framebufferInfo.layers = 1;

        if (const VkResult result = vkCreateFramebuffer(device.GetDevice(), &framebufferInfo, nullptr, &framebuffer);
            result != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }

        const FrameBufferHandle handle = {.Id = framebuffers.size()};

        const VulkanFrameBuffer frameBuffer = {.Framebuffer = framebuffer, .Extent = { .width = width, .height = height }};

        framebuffers.push_back(frameBuffer);

        return handle;
    }

    VulkanFrameBuffer VulkanFrameBufferManager::GetFrameBuffer(const FrameBufferHandle handle) const {
        return framebuffers[handle.Id];
    }
} // Rendering
