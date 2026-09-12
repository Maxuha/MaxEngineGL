//
// Created by zykov on 6/25/2026.
//

#ifndef MAXENGINE_IFRAMEBUFFERMANAGER_H
#define MAXENGINE_IFRAMEBUFFERMANAGER_H


#include "ITextureManager.h"

namespace Rendering {
    struct RenderPassHandle;

    struct FrameBufferHandle {
        uint32_t Id;
    };

    class IFrameBufferManager {
    public:
        virtual ~IFrameBufferManager() = default;

        virtual FrameBufferHandle CreateFramebuffer(RenderPassHandle renderPass, TextureHandle texture) = 0;
    };
} // Rendering

#endif //MAXENGINE_IFRAMEBUFFERMANAGER_H
