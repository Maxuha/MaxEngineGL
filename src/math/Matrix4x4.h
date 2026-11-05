//
// Created by zykov on 3/15/2026.
//

#ifndef MAXENGINE_MATRIX4X4_H
#define MAXENGINE_MATRIX4X4_H

#include <iomanip>
#include <glm/glm.hpp>

#include "Matrix3x3.h"
#include "Vector3.h"
#include "Vector4.h"

// Column-major
class alignas(16) Matrix4x4 {
public:
    Matrix4x4();

    explicit Matrix4x4(const glm::mat4& mat) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                m[i][j] = mat[i][j];
            }
        }
    }

    float m[4][4]{};

    static Matrix4x4 Identity();

    Vector4 GetColumn(int column) const;

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

    static Matrix4x4 LookAt(Vector3 eye, Vector3 target, Vector3 up);

    static Matrix4x4 Perspective(float fov, float aspectRatio, float zNear, float zFar);

    static Matrix4x4 Orthographic(float left, float right, float bottom, float top, float zNear, float zFar);

    Matrix4x4 operator*(const Matrix4x4& m1) const;
    Vector4 operator*(const Vector4& v1) const;
    Vector3 operator*(const Vector3& v1) const;

    Vector4 operator[](unsigned int row) const;

    friend std::ostream& operator<<(std::ostream& os, const Matrix4x4& mat) {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                os << std::setw(10) << mat.m[j][i] << " ";
            }
            os << "\n";
        }
        return os;
    }
};


#endif //MAXENGINE_MATRIX4X4_H
