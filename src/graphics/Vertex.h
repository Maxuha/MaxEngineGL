//
// Created by zykov on 3/17/2026.
//

#ifndef MAXENGINE_VERTEX_H
#define MAXENGINE_VERTEX_H
#include "../math/Vector2.h"
#include "../math/Vector3.h"

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    Vector3 position;
    Vector3 normal;
    Vector2 texCoords{};
    Vector3 tangent;
    Vector3 bitangent;

    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};


#endif //MAXENGINE_VERTEX_H