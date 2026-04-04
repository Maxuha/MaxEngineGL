//
// Created by zykov on 4/3/2026.
//


#include "VertexBuffer.h"
#include "glad/glad.h"

VertexBuffer::VertexBuffer(const void* vertecies, const unsigned size) {
    glGenBuffers(1, &id);
    Bind();
    glBufferData(GL_ARRAY_BUFFER, size, vertecies, GL_STATIC_DRAW);
    Unbind();
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &id);
}

void VertexBuffer::Bind() {
    glBindBuffer(GL_ARRAY_BUFFER, id);
}

void VertexBuffer::Unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
