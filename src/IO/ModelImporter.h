//
// Created by zykov on 3/30/2026.
//

#ifndef MAXENGINE_MESHIMPORTER_H
#define MAXENGINE_MESHIMPORTER_H
#include "../renderer/domain/Model.h"


class ModelImporter {
    public:
    virtual ~ModelImporter() = default;

    virtual Model* Import(const char* fileName) = 0;
};

#endif //MAXENGINE_MESHIMPORTER_H
