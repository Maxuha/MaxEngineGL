//
// Created by zykov on 6/26/2026.
//

#include "OGLFrameManager.h"

#include "CommandBuffer.h"
#include "OGLRenderDevice.h"
#include "../IFrameBufferManager.h"

namespace Rendering {
    OGLFrameManager::OGLFrameManager(OGLRenderDevice &renderDevice) : renderDevice(&renderDevice) {
        commandBuffer = new CommandBuffer();
    }

    OGLFrameManager::~OGLFrameManager() {
    }

    void OGLFrameManager::CreateFrames(RenderPassHandle renderPass, TextureHandle texture) {

    }

    bool OGLFrameManager::PrepareFrame() {
        return true;
    }

    CommandBuffer * OGLFrameManager::GetCommandBufferForCurrentFrame() {
        return commandBuffer;
    }

    void OGLFrameManager::SendFrameToGPU() {
        renderDevice->SubmitCommandBuffer(commandBuffer);
    }

    void OGLFrameManager::Present() {
        renderDevice->Present();
    }

    FrameBufferHandle OGLFrameManager::GetCurrentFrame() {

        return FrameBufferHandle {.Id = 0};
    }
} // Rendering