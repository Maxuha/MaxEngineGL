//
// Created by zykov on 6/26/2026.
//

#include "OGLFrameBufferManager.h"

namespace Rendering {

    OGLFrameBufferManager::OGLFrameBufferManager(RenderContext &context) : context(&context) {
        OGLFrameBuffer handle;
        handle.Framebuffer = 0;
        frameBuffers.push_back(handle);
    }

    OGLFrameBufferManager::~OGLFrameBufferManager() {
    }

    // FrameBufferHandle OGLFrameBufferManager::CreateFramebuffer(const RenderPassHandle renderPass, const TextureHandle texture) {
    //     // GLuint fbo;
    //     // glCreateFramebuffers(1, &fbo);
    //     // glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    //     //
    //     // const OGLTexture tex = static_cast<OGLTextureManager*>(context->TextureManager)->GetTexture(texture);
    //     //
    //     // glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, tex.Id, 0);
    //     //
    //     // if (glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    //     //     // Handle framebuffer creation error
    //     // }
    //     //
    //     // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //     //
    //     // const FrameBufferHandle handle = {.Id = static_cast<uint32_t>(frameBuffers.size())};
    //     //
    //     // const OGLFrameBuffer frameBuffer{fbo };
    //     //
    //     // frameBuffers.push_back(frameBuffer);
    //
    //     // return handle;
    //     return {};
    // }

    OGLFrameBuffer OGLFrameBufferManager::GetFrameBuffer(const FrameBufferHandle frameBuffer) const {
        return frameBuffers[frameBuffer.Id];
    }
} // Rendering