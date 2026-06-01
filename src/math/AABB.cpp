//
// Created by zykov on 4/1/2026.
//

#include "AABB.h"

#include <iostream>
#include <span>
#include "Vertex.h"

bool AABB::Intersect(const AABB &AABB) const {
    return min.x <= AABB.max.x && max.x >= AABB.min.x &&
           min.y <= AABB.max.y && max.y >= AABB.min.y &&
           min.z <= AABB.max.z && max.z >= AABB.min.z;
}

Vector3 AABB::CalculateOverlap(const AABB &a, const AABB &b) {
    const float overlapX = std::min(a.max.x, b.max.x) - std::max(a.min.x, b.min.x);
    const float overlapY = std::min(a.max.y, b.max.y) - std::max(a.min.y, b.min.y);
    const float overlapZ = std::min(a.max.z, b.max.z) - std::max(a.min.z, b.min.z);

    return Vector3(overlapX, overlapY, overlapZ);
}

Vector3 AABB::GetCenter(const std::span<const Vertex> vertices) {
    Vector3 min = vertices[0].position;
    Vector3 max = vertices[0].position;

    for (const auto &vertex: vertices) {
        if (vertex.position.x < min.x) min.x = vertex.position.x;
        if (vertex.position.y < min.y) min.y = vertex.position.y;
        if (vertex.position.z < min.z) min.z = vertex.position.z;

        if (vertex.position.x > max.x) max.x = vertex.position.x;
        if (vertex.position.y > max.y) max.y = vertex.position.y;
        if (vertex.position.z > max.z) max.z = vertex.position.z;
    }

    return (min + max) * 0.5f;
}
