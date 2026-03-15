//
// Created by zykov on 3/15/2026.
//

#include "MeshPrimitives.h"
#include "../math/Vector3.h"
#include "Triangle.h"

Mesh MeshPrimitives::CreateBox() {
    auto box = Mesh();

    box.tries = {
        // SOUTH
        {{{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}}},
        {{{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}}},

        // EAST
        {{{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}}},
        {{{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}}},

        // NORTH
        {{{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}}},
        {{{1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}},

        // WEST
        {{{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}}},
        {{{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}},

        // TOP
        {{{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}}},
        {{{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}}},

        // BOTTOM
        {{{1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}},
        {{{1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}}}
    };

    return box;
}

Mesh MeshPrimitives::CreateLine(Vector3 direction) {
    auto line = Mesh();

    line.tries = {
        // SOUTH
        {{{0.0f, 0.0f, 0.0f}, {0.0f, direction.y, 0.0f}, {direction.x, direction.y, 0.0f}}},
        {{{0.0f, 0.0f, 0.0f}, {direction.x, direction.y, 0.0f}, {direction.x, 0.0f, 0.0f}}},

        // EAST
        {{{direction.x, 0.0f, 0.0f}, {direction.x, direction.y, 0.0f}, {direction.x, direction.y, direction.z}}},
        {{{direction.x, 0.0f, 0.0f}, {direction.x, direction.y, direction.z}, {direction.x, 0.0f, direction.z}}},

        // NORTH
        {{{direction.x, 0.0f, direction.z}, {direction.x, direction.y, direction.z},{0.0f, direction.y, direction.z}}},
        {{{direction.x, 0.0f, direction.z}, {0.0f, direction.y, direction.z}, {0.0f, 0.0f, direction.z}}},

        // WEST
        {{{0.0f, 0.0f, direction.z}, {0.0f, direction.y, direction.z}, {0.0f, direction.y, 0.0f}}},
        {{{0.0f, 0.0f, direction.z}, {0.0f, direction.y, 0.0f}, {0.0f, 0.0f, 0.0f}}},

        // TOP
        {{{0.0f, direction.y, 0.0f}, {0.0f, direction.y, direction.z}, {direction.x, direction.y, direction.z}}},
        {{{0.0f, direction.y, 0.0f}, {direction.x, direction.y, direction.z}, {direction.x, direction.y, 0.0f}}},

        // BOTTOM
        {{{direction.x, 0.0f, direction.z}, {0.0f, 0.0f, direction.z}, {0.0f, 0.0f, 0.0f}}},
        {{{direction.x, 0.0f, direction.z}, {0.0f, 0.0f, 0.0f}, {direction.x, 0.0f, 0.0f}}}
    };

    return line;
}
