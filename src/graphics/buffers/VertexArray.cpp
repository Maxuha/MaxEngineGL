//
// Created by zykov on 4/3/2026.
//


#include "VertexArray.h"
#include "glad/glad.h"

VertexArray::VertexArray() {
    glGenVertexArrays(1, &id);
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &id);
}

void VertexArray::Bind() const {
    glBindVertexArray(id);
}

void VertexArray::Unbind() const {
    glBindVertexArray(0);
}

void VertexArray::AddAttribute(GLuint index, GLint componentCount, GLenum type, bool isNormalized, GLsizei size,  const GLvoid* offset) {
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, componentCount, type, isNormalized, size, offset);
}
