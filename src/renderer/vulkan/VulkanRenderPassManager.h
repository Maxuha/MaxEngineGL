//
// Created by zykov on 6/18/2026.
//

#ifndef MAXENGINE_VULKANRENDERPASS_H
#define MAXENGINE_VULKANRENDERPASS_H

#include <span>
#include "VulkanLogicalDevice.h"
#include "../IRenderPassManager.h"
#include "../domain/struct/PipelineStateDesc.h"


namespace Rendering {
    struct VulkanRenderPass {
        VkRenderPass RenderPass;
        Rect Rect;
    };

    class VulkanRenderPassManager : public IRenderPassManager {
    public:
        explicit VulkanRenderPassManager(ILogicalDevice& device);

        ~VulkanRenderPassManager() override;

        RenderPassHandle CreateRenderPass(std::span<const AttachmentDescription> attachments) override;

        VulkanRenderPass GetRenderPass(RenderPassHandle handle) const;

    private:
        VulkanLogicalDevice* logicalDevice;

        std::vector<VulkanRenderPass> renderPasses;

        VkAttachmentLoadOp MapLoadOp(AttachmentLoadOp loadOp);
        VkAttachmentStoreOp MapStoreOp(AttachmentStoreOp storeOp);
        VkFormat MapImageFormat(ImageFormat imageFormat);
    };
} // Rendering

#endif //MAXENGINE_VULKANRENDERPASS_H
