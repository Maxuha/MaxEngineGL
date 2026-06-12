//
// Created by zykov on 6/12/2026.
//

#include "BufferManager.h"

#include <iostream>

#include "glad/glad.h"

namespace Rendering {
    void BufferManager::CreateBuffer(const BufferDesc &desc, const void *data) {
        // GLuint glId;
        // glCreateBuffers(1, &glId);
        //
        // GLbitfield flags = 0;
        //
        // if (desc.usage == BufferUsage::Uniform) {
        //     flags |= GL_DYNAMIC_STORAGE_BIT;
        // }
        //
        // glNamedBufferStorage(
        //     glId,
        //     static_cast<GLsizeiptr>(desc.size),
        //     data,
        //     flags
        // );
        //
        // const auto id = BufferHandle{.Id = static_cast<uint32_t>(buffers.size())};
        //
        // BufferData bufferData;
        // bufferData.id = glId;
        //
        // buffers.push_back(bufferData);
        // return id;
    }

    BufferHandle BufferManager::CreateFrameBuffer(const TextureData &textureData) {
        GLuint fbo;
        glCreateFramebuffers(1, &fbo);

        glNamedFramebufferTexture(fbo, GL_DEPTH_ATTACHMENT, textureData.id, 0);

        glNamedFramebufferDrawBuffer(fbo, GL_NONE);
        glNamedFramebufferReadBuffer(fbo, GL_NONE);

        const GLenum status = glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "FrameBuffer is not complete: " << status << std::endl;
        }

        const auto id = BufferHandle{.Id = static_cast<uint32_t>(buffers.size())};

        FrameBufferData fboData;
        fboData.id = fbo;

        frameBuffers.push_back(fboData);
        return id;
    }
} // Rendering