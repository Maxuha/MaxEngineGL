//
// Created by zykov on 4/1/2026.
//

#include "AABB.h"

#include <span>
#include "../graphics/Vertex.h"

Vector3 AABB::GetCenter(std::span<const Vertex> vertices) {
    Vector3 min = vertices[0].position;
    Vector3 max = vertices[0].position;

    for (const auto& vertex : vertices) {
        if (vertex.position.x < min.x) min.x = vertex.position.x;
        if (vertex.position.y < min.y) min.y = vertex.position.y;
        if (vertex.position.z < min.z) min.z = vertex.position.z;

        if (vertex.position.x > max.x) max.x = vertex.position.x;
        if (vertex.position.y > max.y) max.y = vertex.position.y;
        if (vertex.position.z > max.z) max.z = vertex.position.z;
    }

    return (min + max) * 0.5f;
}
