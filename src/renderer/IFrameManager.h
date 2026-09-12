//
// Created by zykov on 6/24/2026.
//

#ifndef MAXENGINE_IFRAMEBUFFER_H
#define MAXENGINE_IFRAMEBUFFER_H
#include "gl/CommandBuffer.h"

namespace Rendering {
    struct VulkanFrame;

    class IFrameManager {
    public:
        virtual ~IFrameManager() = default;

        virtual void CreateFrames(RenderPassHandle renderPass, TextureHandle texture) = 0;

        virtual bool PrepareFrame() = 0;

        virtual CommandBuffer* GetCommandBufferForCurrentFrame() = 0;

        virtual void SendFrameToGPU() = 0;

        virtual void Present() = 0;

        virtual FrameBufferHandle GetCurrentFrame() = 0;
    };
} // Rendering

#endif //MAXENGINE_IFRAMEBUFFER_H
