//
// Created by zykov on 3/15/2026.
//

#include "Mesh.h"

#include "Texture.h"
#include "buffers/VertexArray.h"
#include "../math/AABB.h"
#include "buffers/VertexBuffer.h"
#include "glad/glad.h"

Mesh::Mesh(std::span<Vertex> vertices, std::span<const unsigned int> indices) : Size(indices.size()) {
    vertexArray = new VertexArray();
    vertexArray->Bind();

    vertexBuffer = new VertexBuffer(vertices.data(), vertices.size_bytes());
    vertexBuffer->Bind();

    indexBuffer = new IndexBuffer(indices.data(), indices.size_bytes());
    indexBuffer->Bind();

    vertexArray->AddAttribute(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    vertexArray->AddAttribute(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, normal)));
    vertexArray->AddAttribute(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, texCoords)));

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
