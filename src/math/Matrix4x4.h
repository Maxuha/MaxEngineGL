//
// Created by zykov on 3/15/2026.
//

#ifndef MAXENGINE_MATRIX4X4_H
#define MAXENGINE_MATRIX4X4_H

#include <glm/glm.hpp>

#include "Matrix3x3.h"
#include "Vector3.h"
#include "Vector4.h"

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

    float m[4][4]{};

    Matrix4x4 Identity();

    static Matrix4x4 Rotate(Vector3 axis);
    static Matrix4x4 RotateX(float angle);
    static Matrix4x4 RotateY(float angle);
    static Matrix4x4 RotateZ(float angle);

    static Matrix4x4 Transform(Vector3 translation);

    static Matrix4x4 Scale(Vector3 scale);

    Matrix4x4 Transpose() const;
    Matrix4x4 Inverse() const;
    Matrix3x3 Minor(int column, int row) const;
    float Determinant() const;

    Matrix4x4 operator*(const Matrix4x4& m1) const;
    Vector4 operator*(const Vector4& v1) const;
    Vector3 operator*(const Vector3& v1) const;
};


#endif //MAXENGINE_MATRIX4X4_H
