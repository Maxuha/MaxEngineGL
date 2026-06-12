//
// Created by zykov on 6/5/2026.
//

#ifndef MAXENGINE_MATERIALIMPORTER_H
#define MAXENGINE_MATERIALIMPORTER_H
#include "IMaterialImporter.h"


class MaterialImporter : public IMaterialImporter {
public:
    virtual ~MaterialImporter() = default;

    virtual Material *Import() override;
};


#endif //MAXENGINE_MATERIALIMPORTER_H
