//
// Created by zykov on 6/26/2026.
//

#ifndef MAXENGINE_OGLFRAMEMANAGER_H
#define MAXENGINE_OGLFRAMEMANAGER_H
#include "CommandBuffer.h"
#include "../IFrameManager.h"

namespace Rendering {
    class OGLRenderDevice;

    class OGLFrameManager : public IFrameManager {
    public:
        explicit OGLFrameManager(OGLRenderDevice &renderDevice);

        ~OGLFrameManager() override;

        void CreateFrames(RenderPassHandle renderPass, TextureHandle texture) override;

        bool PrepareFrame() override;

        CommandBuffer * GetCommandBufferForCurrentFrame() override;

        void SendFrameToGPU() override;

        void Present() override;

        FrameBufferHandle GetCurrentFrame() override;

    private:
        OGLRenderDevice* renderDevice;
        CommandBuffer *commandBuffer;

    };
} // Rendering

#endif //MAXENGINE_OGLFRAMEMANAGER_H
