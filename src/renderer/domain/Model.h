//
// Created by zykov on 4/26/2026.
//

#ifndef MAXENGINE_MODEL_H
#define MAXENGINE_MODEL_H
#include <memory>
#include <vector>

#include "Material.h"
#include "Mesh.h"
#include "../../math/Vector3.h"

struct MeshEntry {
    std::shared_ptr<Mesh> mesh;
    Vector3 position;
    Vector3 rotation; // euler angles in degrees
    Vector3 scale;
    unsigned int materialIndex;
};

class Model {
public:
    std::vector<MeshEntry> meshes;
    std::vector<Material> materials;
    GameObject* root;
};


#endif //MAXENGINE_MODEL_H
