//
// Created by zykov on 3/15/2026.
//

#include "Mesh.h"

#include "buffers/VertexArray.h"
#include "../math/AABB.h"
#include "buffers/VertexBuffer.h"
#include "glad/glad.h"

Mesh::Mesh(const std::span<const Vertex> vertices, const std::span<const unsigned int> indices) : Size(vertices.size_bytes()) {
    center = AABB::GetCenter(vertices);

    vertexArray = new VertexArray();
    vertexArray->Bind();

    vertexBuffer = new VertexBuffer(vertices.data(), vertices.size_bytes());
    vertexBuffer->Bind();

    indexBuffer = new IndexBuffer(indices.data(), indices.size_bytes());
    indexBuffer->Bind();

    vertexArray->AddAttribute(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    vertexArray->AddAttribute(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));

    vertexArray->Unbind();
    vertexBuffer->Unbind();
    indexBuffer->Unbind();
}

void Mesh::Bind() const {
    vertexArray->Bind();
    indexBuffer->Bind();
}

void Mesh::Unbind() const {
    vertexArray->Unbind();
    indexBuffer->Unbind();
}

unsigned int Mesh::GetIndexCount() const {
    return Size;
}
