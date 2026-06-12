//
// Created by zykov on 3/15/2026.
//

#include "Mesh.h"

#include "../../math/AABB.h"

Mesh::Mesh(const std::span<const Vertex> &vertices, const std::span<const uint32_t>& indices) : Size(indices.size()), vertices(vertices), indices(indices) {
    this->vertices = vertices;
    this->indices = indices;
}

size_t Mesh::GetIndexCount() const {
    return Size;
}

MeshId Mesh::GetId() const {
    return id;
}

const std::span<const Vertex>& Mesh::GetVertices() const {
    return vertices;
}

const std::span<const uint32_t>& Mesh::GetIndices() const {
    return indices;
}
