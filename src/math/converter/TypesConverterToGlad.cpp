//
// Created by zykov on 3/22/2026.
//

#include "TypesConverterToGlad.h"
#include <glm/glm.hpp>

glm::mat4 TypesConverterToGlad::convertMat4x4(const Matrix4x4 &mat) {
    auto result = glm::mat4();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result[i][j] = mat.m[i][j];
        }
    }
    return result;
}

glm::vec3 TypesConverterToGlad::convertVec3(const Vector3 vec) {
    auto result = glm::vec3();
    result.x = vec.x;
    result.y = vec.y;
    result.z = vec.z;
    return result;
}
