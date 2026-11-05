
//
// Created by zykov on 3/15/2026.
//

#include "Matrix4x4.h"
#include <cmath>
#include "Matrix3x3.h"
#include "Vector4.h"

Matrix4x4::Matrix4x4() {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            m[i][j] = 0.0f;
        }
    }
}

Matrix4x4 Matrix4x4::Identity() {
    Matrix4x4 matrix;
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    return matrix;
}

Vector4 Matrix4x4::GetColumn(const int column) const {
    return Vector4(m[column][0], m[column][1], m[column][2], m[column][3]);
}

Vector4 Matrix4x4::operator[](const unsigned int column) const {
    return Vector4(m[column][0], m[column][1], m[column][2], m[column][3]);
}

Matrix4x4 Matrix4x4::Rotate(const Vector3 axis) {
    return RotateZ(axis.z) * RotateY(axis.y) * RotateX(axis.x);
}

Matrix4x4 Matrix4x4::RotateX(const float angle) {
    auto s = std::sin(glm::radians(angle));
    auto c = std::cos(glm::radians(angle));
    auto temp = Identity();

    temp.m[1][1] = c;  temp.m[1][2] = s;
    temp.m[2][1] = -s; temp.m[2][2] = c;
    return temp;
}

Matrix4x4 Matrix4x4::RotateY(const float angle) {
    auto s = std::sin(glm::radians(angle));
    auto c = std::cos(glm::radians(angle));
    auto temp = Identity();

    temp.m[0][0] = c;  temp.m[0][2] = -s;
    temp.m[2][0] = s;  temp.m[2][2] = c;
    return temp;
}

Matrix4x4 Matrix4x4::RotateZ(const float angle) {
    auto s = std::sin(glm::radians(angle));
    auto c = std::cos(glm::radians(angle));
    auto temp = Identity();

    temp.m[0][0] = c;  temp.m[0][1] = s;
    temp.m[1][0] = -s; temp.m[1][1] = c;
    return temp;
}

Matrix4x4 Matrix4x4::Transform(const Vector3 translation) {
    auto result = Identity();
    result.m[3][0] = translation.x;
    result.m[3][1] = translation.y;
    result.m[3][2] = translation.z;
    return result;
}

Matrix4x4 Matrix4x4::Scale(const Vector3 scale) {
    auto result = Identity();
    result.m[0][0] = scale.x;
    result.m[1][1] = scale.y;
    result.m[2][2] = scale.z;
    return result;
}


Matrix4x4 Matrix4x4::operator*(const Matrix4x4 &m1) const {
    auto temp = Matrix4x4();
    for (int c = 0; c < 4; c++) {
        for (int r = 0; r < 4; r++) {
            temp.m[c][r] = m[0][r] * m1.m[c][0] +
                           m[1][r] * m1.m[c][1] +
                           m[2][r] * m1.m[c][2] +
                           m[3][r] * m1.m[c][3];
        }
    }
    return temp;
}

Vector4 Matrix4x4::operator*(const Vector4 &v1) const {
    return Vector4(
        m[0][0] * v1.x + m[1][0] * v1.y + m[2][0] * v1.z + m[3][0] * v1.w,
        m[0][1] * v1.x + m[1][1] * v1.y + m[2][1] * v1.z + m[3][1] * v1.w,
        m[0][2] * v1.x + m[1][2] * v1.y + m[2][2] * v1.z + m[3][2] * v1.w,
        m[0][3] * v1.x + m[1][3] * v1.y + m[2][3] * v1.z + m[3][3] * v1.w
    );
}

Vector3 Matrix4x4::operator*(const Vector3 &v1) const {
    float w = m[0][3] * v1.x + m[1][3] * v1.y + m[2][3] * v1.z + m[3][3];

    float invW = (w != 0.0f) ? (1.0f / w) : 1.0f;

    return Vector3(
        (m[0][0] * v1.x + m[1][0] * v1.y + m[2][0] * v1.z + m[3][0]) * invW,
        (m[0][1] * v1.x + m[1][1] * v1.y + m[2][1] * v1.z + m[3][1]) * invW,
        (m[0][2] * v1.x + m[1][2] * v1.y + m[2][2] * v1.z + m[3][2]) * invW
    );
}

Matrix4x4 Matrix4x4::Transpose() const {
    auto temp = Matrix4x4();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            temp.m[i][j] = m[j][i];
        }
    }
    return temp;
}

Matrix4x4 Matrix4x4::Inverse() const {
    float det = Determinant();
    if (std::abs(det) < 1e-6f) {
        return Identity();
    }

    Matrix4x4 result = Matrix4x4();
    for (int c = 0; c < 4; c++) {
        for (int r = 0; r < 4; r++) {
            float sign = ((c + r) % 2 == 0) ? 1.0f : -1.0f;
            result.m[c][r] = sign * Minor(r, c).Determinant();
        }
    }

    float invDet = 1.0f / det;
    for (int c = 0; c < 4; c++) {
        for (int r = 0; r < 4; r++) {
            result.m[c][r] *= invDet;
        }
    }

    return result;
}

Matrix3x3 Matrix4x4::Minor(const int column, const int row) const {
    Matrix3x3 result;
    int subCol = 0;
    for (int i = 0; i < 4; i++) {
        if (i == column) continue;
        int subRow = 0;
        for (int j = 0; j < 4; j++) {
            if (j == row) continue;
            result.m[subCol][subRow] = m[i][j];
            subRow++;
        }
        subCol++;
    }
    return result;
}

float Matrix4x4::Determinant() const {
    float det = 0.0f;
    float sign = 1.0f;
    for (int k = 0; k < 4; k++) {
        det += sign * m[k][0] * Minor(k, 0).Determinant();
        sign = -sign;
    }
    return det;
}

Matrix4x4 Matrix4x4::LookAt(const Vector3 eye, const Vector3 target, const Vector3 up) {
    auto matrix = Identity();

    const Vector3 forward = (target - eye).Normalize();
    const Vector3 right = Vector3::CrossProduct(up, forward).Normalize();
    const Vector3 _up = Vector3::CrossProduct(forward, right).Normalize();
    const auto position = -Vector3(Vector3::DotProduct(right, eye), Vector3::DotProduct(up, eye), Vector3::DotProduct(forward, eye));

    matrix.m[0][0] = right.x;
    matrix.m[1][0] = right.y;
    matrix.m[2][0] = right.z;

    matrix.m[0][1] = _up.x;
    matrix.m[1][1] = _up.y;
    matrix.m[2][1] = _up.z;

    matrix.m[0][2] = forward.x;
    matrix.m[1][2] = forward.y;
    matrix.m[2][2] = forward.z;

    matrix.m[3][0] = position.x;
    matrix.m[3][1] = position.y;
    matrix.m[3][2] = position.z;

    // std::cout << "matrix: " << matrix << std::endl;

    return matrix;
}

Matrix4x4 Matrix4x4::Perspective(const float fov, const float aspectRatio, const float zNear, const float zFar) {
    auto matrix = Identity();

    const float fovRad = fov * (Pi / 180.0f);
    const float tanHalfFov = std::tan(fovRad / 2);
    const float f = 1.0f / tanHalfFov;

    matrix.m[0][0] = f / aspectRatio;
    matrix.m[1][1] = -f;
    matrix.m[2][2] = zFar / (zFar - zNear);
    matrix.m[3][2] = -(zFar * zNear) / (zFar - zNear);
    matrix.m[2][3] = 1.0f;
    matrix.m[3][3] = 0.0f;

    return matrix;
}

Matrix4x4 Matrix4x4::Orthographic(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar) {
    auto matrix = Identity();

    const float width = right - left;
    const float height = top - bottom;
    const float depth = zFar - zNear;

    matrix.m[0][0] = 2.0f / width;
    matrix.m[1][1] = -2.0f / height;
    matrix.m[2][2] = 1.0f / depth;
    matrix.m[0][3] = -(right + left) / width;
    matrix.m[1][3] = (top + bottom) / height;
    matrix.m[2][3] = -zNear / depth;

    return matrix;
}
