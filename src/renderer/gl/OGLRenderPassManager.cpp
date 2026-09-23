//
// Created by zykov on 6/26/2026.
//

#include "OGLRenderPassManager.h"


namespace Rendering {
    OGLRenderPassManager::OGLRenderPassManager() {
    }

    OGLRenderPassManager::~OGLRenderPassManager() {
    }

    RenderPassHandle OGLRenderPassManager::CreateRenderPass(const std::span<const AttachmentDescription> attachments) {
        const RenderPassHandle renderPassHandle = { .Id = renderPasses.size() };

        // OGLRenderPass renderPass;
        // renderPass.Attachment = attachments.back();
        // renderPass.rect = { .x = 0, .y = 0, .width = 1280, .height = 720 };

       // renderPasses.push_back(renderPass);

        return renderPassHandle;
    }

    OGLRenderPass OGLRenderPassManager::GetRenderPass(const RenderPassHandle handle) const {
        return renderPasses[handle.Id];
    }
} // Rendering