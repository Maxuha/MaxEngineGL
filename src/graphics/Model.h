//
// Created by zykov on 4/26/2026.
//

#ifndef MAXENGINE_MODEL_H
#define MAXENGINE_MODEL_H
#include <vector>

#include "Mesh.h"
#include "../math/Vector3.h"

struct MeshEntry {
    Mesh* mesh;
    Vector3 position;
    Vector3 rotation; // euler angles in degrees
    Vector3 scale;
};

class Model {
public:
    std::vector<MeshEntry> meshes;
};


#endif //MAXENGINE_MODEL_H
