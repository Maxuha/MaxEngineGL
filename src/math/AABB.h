//
// Created by zykov on 4/1/2026.
//

#ifndef MAXENGINE_AABB_H
#define MAXENGINE_AABB_H
#include <span>

#include "Vector3.h"
#include "../graphics/Vertex.h"

class AABB {
public:
    AABB() {
        min = Vector3(0, 0, 0);
        max = Vector3(1, 1, 1);
    }

    Vector3 min{}, max{};

    bool Intersect(const AABB& AABB) const;
    static Vector3 CalculateOverlap(const AABB &a, const AABB &b);
    static Vector3 GetCenter(std::span<const Vertex> vertices);
};

#endif //MAXENGINE_AABB_H
