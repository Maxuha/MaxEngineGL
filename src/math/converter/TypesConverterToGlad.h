//
// Created by zykov on 3/22/2026.
//

#ifndef MAXENGINE_TYPESCONVERTERTOGLAD_H
#define MAXENGINE_TYPESCONVERTERTOGLAD_H
#include "../Matrix4x4.h"
#include "../Vector3.h"
#include "glm/fwd.hpp"
#include "glm/detail/type_mat4x2.hpp"


class TypesConverterToGlad {
    public:
        static glm::mat4 convertMat4x4(const Matrix4x4 &mat);
        static glm::vec3 convertVec3(Vector3 vec);
};


#endif //MAXENGINE_TYPESCONVERTERTOGLAD_H