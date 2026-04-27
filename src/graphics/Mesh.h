//
// Created by zykov on 3/15/2026.
//

#ifndef MAXENGINE_MESH_H
#define MAXENGINE_MESH_H
#include <span>
#include "Vertex.h"
#include "buffers/IndexBuffer.h"
#include "buffers/VertexArray.h"
#include "buffers/VertexBuffer.h"

class Texture;

class Mesh {
public:
    explicit Mesh(std::span<Vertex> vertices, std::span<const unsigned int> indices);

    ~Mesh() {
        delete vertexArray;
        delete vertexBuffer;
        delete indexBuffer;
    }

    unsigned int Size;
    Vector3 center{};
    Vector3 position{};

    VertexArray* vertexArray;
    VertexBuffer* vertexBuffer;
    IndexBuffer* indexBuffer;

    void Bind() const;
    void Unbind() const;

    unsigned int GetIndexCount() const;
};


#endif //MAXENGINE_MESH_H