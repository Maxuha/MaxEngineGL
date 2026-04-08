//
// Created by zykov on 3/30/2026.
//

#ifndef MAXENGINE_MESHIMPORTER_H
#define MAXENGINE_MESHIMPORTER_H
#include "../graphics/Mesh.h"


class ModelImporter {
    public:
    virtual Mesh* Import(const char* fileName) = 0;
};

#endif //MAXENGINE_MESHIMPORTER_H
