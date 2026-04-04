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
    static Vector3 GetCenter(std::span<const Vertex> vertices);
};

#endif //MAXENGINE_AABB_H
