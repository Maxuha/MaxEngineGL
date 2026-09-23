//
// Created by zykov on 6/25/2026.
//

#ifndef MAXENGINE_IRENDERPASS_H
#define MAXENGINE_IRENDERPASS_H

namespace Rendering {
    class CommandBuffer;
    struct FrameBufferHandle;
    struct PipelineHandle;
    struct RenderPassHandle;

    class IRenderPass {
    public:
        virtual ~IRenderPass() = default;

        virtual RenderPassHandle GetHandle() = 0;

        virtual void Render(CommandBuffer &commandBuffer, FrameBufferHandle frameBuffer, PipelineHandle pipeline) = 0;
    };
} // Rendering

#endif //MAXENGINE_IRENDERPASS_H
