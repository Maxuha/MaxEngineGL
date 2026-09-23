//
// Created by zykov on 6/25/2026.
//

#ifndef MAXENGINE_IRENDERPASSMANAGER_H
#define MAXENGINE_IRENDERPASSMANAGER_H
#include <span>


namespace Rendering {
    struct AttachmentDescription;

    struct RenderPassHandle {
        uint32_t Id;
    };

    class IRenderPassManager {
    public:
        virtual ~IRenderPassManager() = default;

        virtual RenderPassHandle CreateRenderPass(std::span<const AttachmentDescription> attachments) = 0;
    };
} // Rendering

#endif //MAXENGINE_IRENDERPASSMANAGER_H
