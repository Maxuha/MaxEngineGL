//
// Created by zykov on 4/1/2026.
//

#include "AABB.h"

#include <span>
#include "../graphics/Vertex.h"

Vector3 AABB::GetCenter(std::span<const Vertex> vertices) {
    Vector3 min = vertices[0].position;
    Vector3 max = vertices[0].position;

    for (const auto&[position, normal] : vertices) {
        if (position.x < min.x) min.x = position.x;
        if (position.y < min.y) min.y = position.y;
        if (position.z < min.z) min.z = position.z;

        if (position.x > max.x) max.x = position.x;
        if (position.y > max.y) max.y = position.y;
        if (position.z > max.z) max.z = position.z;
    }

    return (min + max) * 0.5f;
}
