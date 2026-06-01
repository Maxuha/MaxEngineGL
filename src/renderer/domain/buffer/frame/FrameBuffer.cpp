//
// Created by zykov on 5/11/2026.
//

#include "FrameBuffer.h"

#include <iostream>
#include <ostream>

#include "glad/glad.h"

Rendering::FrameBuffer::FrameBuffer() {
    glCreateFramebuffers(1, &id);
}

size_t Rendering::FrameBuffer::GetWidth() const {
    return width;
}

size_t Rendering::FrameBuffer::GetHeight() const {
    return height;
}

void Rendering::FrameBuffer::BindTexture(const GLTexture& texture) {
    width = texture.GetWidth();
    height = texture.GetHeight();

    glNamedFramebufferTexture(id, GL_DEPTH_ATTACHMENT, 1, 0);

    glNamedFramebufferDrawBuffer(id, GL_NONE);
    glNamedFramebufferReadBuffer(id, GL_NONE);

    const GLenum status = glCheckNamedFramebufferStatus(id, GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "FrameBuffer is not complete: " << status << std::endl;
        // ????????? ??????
    }
}

void Rendering::FrameBuffer::Bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void Rendering::FrameBuffer::Unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint Rendering::FrameBuffer::GetId() const {
    return id;
}
