//
// Created by zykov on 3/15/2026.
//

#ifndef MAXENGINE_MATRIX4X4_H
#define MAXENGINE_MATRIX4X4_H

#include <glm/glm.hpp>

class Matrix4x4 {
public:
    Matrix4x4(float value);

    template <typename T>
    T Convert() {
        auto result = glm::mat4();
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result[i][j] = m[i][j];
            }
        }
        return result;
    }

    float m[4][4];
};


#endif //MAXENGINE_MATRIX4X4_H
