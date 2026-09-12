//
// Created by zykov on 6/26/2026.
//

#ifndef MAXENGINE_OGLRENDERPASSMANAGER_H
#define MAXENGINE_OGLRENDERPASSMANAGER_H

#include "../IRenderPassManager.h"

namespace Rendering {
    struct OGLRenderPass {
        // AttachmentDescription Attachment;
        // Rect rect{};
    };

    class OGLRenderPassManager : public IRenderPassManager {
        public:
        explicit OGLRenderPassManager();
        ~OGLRenderPassManager() override;

        RenderPassHandle CreateRenderPass(std::span<const AttachmentDescription> attachments) override;

        OGLRenderPass GetRenderPass(RenderPassHandle handle) const;

    private:
        std::vector<OGLRenderPass> renderPasses;
    };
} // Rendering

#endif //MAXENGINE_OGLRENDERPASSMANAGER_H
