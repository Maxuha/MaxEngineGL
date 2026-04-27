//
// Created by zykov on 3/27/2026.
//

#ifndef MAXENGINE_OBJIMPRORTER_H
#define MAXENGINE_OBJIMPRORTER_H
#include "ModelImporter.h"

class ObjImporter : public ModelImporter {
public:
    Model* Import(const char* fileName) override;
};

#endif //MAXENGINE_OBJIMPRORTER_H
