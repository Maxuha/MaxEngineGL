//
// Created by zykov on 3/15/2026.
//

#ifndef MAXENGINE_MESH_H
#define MAXENGINE_MESH_H
#include <vector>
#include "Triangle.h"
#include "../IO/MeshImporter.h"

class Mesh {
public:
    std::vector<Triangle> tries;
    std::vector<Vertex> vertices;
};


#endif //MAXENGINE_MESH_H