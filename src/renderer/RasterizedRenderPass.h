//
// Created by zykov on 6/25/2026.
//

#ifndef MAXENGINE_RASTERIZEDRENDERPASS_H
#define MAXENGINE_RASTERIZEDRENDERPASS_H
#include "IRenderPass.h"
#include "../src/renderer/domain/struct/PipelineStateDesc.h"

namespace Rendering {
    class RasterizedRenderPass : public IRenderPass {
    public:
        explicit RasterizedRenderPass(ImageFormat imageFormat);

        ~RasterizedRenderPass() override = default;

        RenderPassHandle GetHandle() override;

        void Render(CommandBuffer &commandBuffer, FrameBufferHandle frameBuffer, PipelineHandle pipeline) override;

    private:
        IRenderPassManager *renderPassManager;
        RenderPassHandle renderPass;
        Rect viewport;
        Color clearColor;
    };
} // Rendering

#endif //MAXENGINE_RASTERIZEDRENDERPASS_H
