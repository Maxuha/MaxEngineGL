//
// Created by zykov on 5/20/2026.
//

#ifndef MAXENGINE_MESHCREATEDESC_H
#define MAXENGINE_MESHCREATEDESC_H
#include <vector>

#include "../../../math/Vertex.h"


struct MeshCreateDesc {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};


#endif //MAXENGINE_MESHCREATEDESC_H
