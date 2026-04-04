//
// Created by zykov on 3/15/2026.
//

#ifndef MAXENGINE_MESHPRIMITIVES_H
#define MAXENGINE_MESHPRIMITIVES_H
#include "Mesh.h"

class Vector3;

class MeshPrimitives {
public:
    MeshPrimitives() = delete;

    static Mesh* CreateBox();

    static Mesh CreateLine(Vector3 direction);
};

#endif //MAXENGINE_MESHPRIMITIVES_H