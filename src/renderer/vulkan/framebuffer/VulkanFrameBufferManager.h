//
// Created by zykov on 6/19/2026.
//

#ifndef MAXENGINE_VULKANFRAMEBUFFERMANAGER_H
#define MAXENGINE_VULKANFRAMEBUFFERMANAGER_H
#include "IVulkanFramebufferManager.h"
#include "../../IFrameBufferManager.h"
#include "../../RenderContext.h"

namespace Rendering {
    struct RenderContext;
    struct RenderPassHandle;
    class VulkanTextureManager;
    class VulkanLogicalDevice;
    class VulkanRenderPassManager;

    struct VulkanFrameBuffer {
        VkFramebuffer Framebuffer;
        VkExtent2D Extent;
    };

    class VulkanFrameBufferManager : public IFrameBufferManager{
    public:
        explicit VulkanFrameBufferManager(
            VulkanLogicalDevice &device,
            VulkanRenderPassManager &renderPassManager,
            VulkanTextureManager &textureManager
        );

        FrameBufferHandle CreateFramebuffer(RenderPassHandle renderPass, TextureHandle texture) override;

        VulkanFrameBuffer GetFrameBuffer(FrameBufferHandle handle) const;

    private:
        VulkanLogicalDevice& device;
        VulkanRenderPassManager& renderPassManager;
        VulkanTextureManager& textureManager;

        std::vector<VulkanFrameBuffer> framebuffers;
    };
} // Rendering

#endif //MAXENGINE_VULKANFRAMEBUFFERMANAGER_H
