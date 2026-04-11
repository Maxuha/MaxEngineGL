//
// Created by zykov on 4/3/2026.
//

#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(const void* indices, const unsigned count) : count(count) {
    glGenBuffers(1, &id);
    Bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count, indices, GL_STATIC_DRAW);
    Unbind();
}

IndexBuffer::~IndexBuffer() {
    glDeleteBuffers(1, &id);
}

void IndexBuffer::Bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void IndexBuffer::Unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
