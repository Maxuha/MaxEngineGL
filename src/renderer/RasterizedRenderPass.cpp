//
// Created by zykov on 6/25/2026.
//

#include "RasterizedRenderPass.h"

#include "gl/OGLRenderPassManager.h"

namespace Rendering {
    RasterizedRenderPass::RasterizedRenderPass(const ImageFormat imageFormat) : renderPassManager(nullptr) {
        std::vector attachments = {
            AttachmentDescription{
                .format = imageFormat,
                .type = AttachmentType::COLOR,
                .loadOp = AttachmentLoadOp::CLEAR,
                .storeOp = AttachmentStoreOp::STORE
            },
            AttachmentDescription {
                .format = ImageFormat::D32_Sfloat_S8_Uint,
                .type = AttachmentType::DEPTH,
                .loadOp = AttachmentLoadOp::CLEAR,
                .storeOp = AttachmentStoreOp::DONT_CARE
            }
        };

        viewport = Rect{0, 0, 2160, 1440};
        clearColor = Color{0.5f, 0.5f, 0.5f, 1.0f};

        //renderPass = context.RenderPassManager->CreateRenderPass(attachments);
    }

    RenderPassHandle RasterizedRenderPass::GetHandle() {
        return renderPass;
    }

    void RasterizedRenderPass::Render(CommandBuffer &commandBuffer, const FrameBufferHandle frameBuffer, const PipelineHandle pipeline) {
        commandBuffer.BeginRenderPass(renderPass, frameBuffer, viewport, clearColor);
        commandBuffer.BindPipeline(pipeline);
        commandBuffer.Draw(3);
        commandBuffer.EndRenderPass();
    }
} // Rendering
