//
// Created by zykov on 6/18/2026.
//

#include "VulkanRenderPassManager.h"

namespace Rendering {
    VulkanRenderPassManager::VulkanRenderPassManager(ILogicalDevice &device) : logicalDevice(static_cast<VulkanLogicalDevice *>(&device)) {
    }

    VulkanRenderPassManager::~VulkanRenderPassManager() {
        // for (const auto renderPass : renderPasses) {
        //     vkDestroyRenderPass(logicalDevice->GetDevice(), renderPass.RenderPass, nullptr);
        // }
    }

    RenderPassHandle VulkanRenderPassManager::CreateRenderPass(const std::span<const AttachmentDescription> attachments) {
        std::vector<VkAttachmentDescription> vkAttachments(attachments.size());
        std::vector<VkAttachmentReference> colorAttachmentRefs;
        VkAttachmentReference depthAttachmentRef{};
        bool hasDepth = false;

        for (size_t i = 0; i < attachments.size(); i++) {
            const AttachmentDescription attachment = attachments[i];
            VkAttachmentDescription& vkAttachment = vkAttachments[i];

            vkAttachment.format = MapImageFormat(attachment.format);
            vkAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            vkAttachment.loadOp = MapLoadOp(attachment.loadOp);
            vkAttachment.storeOp = MapStoreOp(attachment.storeOp);
            vkAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            vkAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            vkAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            vkAttachment.finalLayout =
                attachment.type == AttachmentType::DEPTH
                    ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
                    : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            if (attachment.type  == AttachmentType::DEPTH) {
                depthAttachmentRef.attachment = static_cast<uint32_t>(i);
                depthAttachmentRef.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                hasDepth = true;
            } else {
                VkAttachmentReference colorAttachmentRef;
                colorAttachmentRef.attachment = static_cast<uint32_t>(i);
                colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                colorAttachmentRefs.push_back(colorAttachmentRef);
            }
        }

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentRefs.size());
        subpass.pColorAttachments = colorAttachmentRefs.data();
        subpass.pDepthStencilAttachment = hasDepth ? &depthAttachmentRef : nullptr;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask =  VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(vkAttachments.size());
        renderPassInfo.pAttachments = vkAttachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        VkRenderPass renderPass = VK_NULL_HANDLE;

        if (vkCreateRenderPass(logicalDevice->GetDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }

        const RenderPassHandle handle = { .Id = renderPasses.size()};

        const VulkanRenderPass renderPassObject = { .RenderPass = renderPass, .Rect = { .x = 0, .y = 0, .width = 1280, .height = 720 } };

        renderPasses.push_back(renderPassObject);

        return handle;
    }

    VulkanRenderPass VulkanRenderPassManager::GetRenderPass(RenderPassHandle handle) const {
        return renderPasses[handle.Id];
    }

    VkAttachmentLoadOp VulkanRenderPassManager::MapLoadOp(const AttachmentLoadOp loadOp) {
        VkAttachmentLoadOp vkLoadOp = {};

        switch (loadOp) {
            case AttachmentLoadOp::LOAD:
                vkLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
                break;
            case AttachmentLoadOp::CLEAR:
                vkLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                break;
            case AttachmentLoadOp::DONT_CARE:
                vkLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                break;
        }
        return vkLoadOp;
    }

    VkAttachmentStoreOp VulkanRenderPassManager::MapStoreOp(const AttachmentStoreOp storeOp) {
        VkAttachmentStoreOp vkStoreOp = {};

        switch (storeOp) {
            case AttachmentStoreOp::STORE:
                vkStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
                break;
            case AttachmentStoreOp::DONT_CARE:
                vkStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                break;
        }
        return vkStoreOp;
    }

    VkFormat VulkanRenderPassManager::MapImageFormat(ImageFormat imageFormat) {
        VkFormat vkFormat = {};

        switch (imageFormat) {
            case ImageFormat::Undefined:
                vkFormat = VK_FORMAT_UNDEFINED;
                break;
            case ImageFormat::BGRA8_Srgb:
                vkFormat = VK_FORMAT_B8G8R8A8_SRGB;
                break;
            case ImageFormat::D24_Unorm_S8_Uint:
                vkFormat = VK_FORMAT_D24_UNORM_S8_UINT;
                break;
            case ImageFormat::D32_Sfloat:
                vkFormat = VK_FORMAT_D32_SFLOAT;
                break;
            case ImageFormat::D32_Sfloat_S8_Uint:
                vkFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;
                break;
            case ImageFormat::RGBA8_Srgb:
                vkFormat = VK_FORMAT_R8G8B8A8_SRGB;
                break;
        }

        return vkFormat;
    }
} // Rendering
