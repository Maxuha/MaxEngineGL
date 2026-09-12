//
// Created by zykov on 3/15/2026.
//

#include "Mesh.h"

#include "../../math/AABB.h"

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<uint16_t>& indices) : Size(indices.size()), vertices(vertices), indices(indices) {
    this->vertices = vertices;
    this->indices = indices;

}

size_t Mesh::GetIndexCount() const {
    return Size;
}

MeshId Mesh::GetId() const {
    return id;
}

std::vector<Vertex>& Mesh::GetVertices() {
    return vertices;
}

std::vector<uint16_t>& Mesh::GetIndices() {
    return indices;
}
