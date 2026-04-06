//
// Created by zykov on 3/27/2026.
//

#ifndef MAXENGINE_OBJIMPRORTER_H
#define MAXENGINE_OBJIMPRORTER_H
#include "ModelImporter.h"

class ObjImporter : public ModelImporter {
public:
    explicit ObjImporter(const char* fileName) {
        this->fileName = fileName;
    }

    const char* fileName;

    Mesh* Import() override;
};

#endif //MAXENGINE_OBJIMPRORTER_H
