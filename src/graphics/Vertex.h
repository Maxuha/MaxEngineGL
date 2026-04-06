//
// Created by zykov on 3/17/2026.
//

#ifndef MAXENGINE_VERTEX_H
#define MAXENGINE_VERTEX_H
#include "../math/Vector2.h"
#include "../math/Vector3.h"


class Vertex {
    public:
    Vector3 position;
    Vector3 normal;
    Vector2 uv;
};


#endif //MAXENGINE_VERTEX_H