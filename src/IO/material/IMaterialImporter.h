//
// Created by zykov on 6/5/2026.
//

#ifndef MAXENGINE_IMATERIALIMPORTER_H
#define MAXENGINE_IMATERIALIMPORTER_H
#include "../../renderer/domain/Material.h"


class IMaterialImporter {
public:
    virtual ~IMaterialImporter() = default;

    virtual Material *Import() = 0;
};


#endif //MAXENGINE_IMATERIALIMPORTER_H
