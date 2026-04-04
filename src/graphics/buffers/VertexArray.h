//
// Created by zykov on 4/3/2026.
//

#ifndef MAXENGINE_VERTEXARRAY_H
#define MAXENGINE_VERTEXARRAY_H
#include "glad/glad.h"

class VertexArray {
public:
    VertexArray();

    ~VertexArray();

    unsigned int id{};

    void Bind() const;

    void Unbind() const;

    void AddAttribute(GLuint index, GLint componentCount, GLenum type, bool isNormalized, GLsizei size,  const GLvoid* offset);
};

#endif //MAXENGINE_VERTEXARRAY_H
