
//
// Created by zykov on 3/15/2026.
//

#include "Matrix4x4.h"
#include <iostream>
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

Matrix4x4::Matrix4x4(const float value) : Matrix4x4() {
    if (value == 1.0f) {
        Identity();
    }
}

Matrix4x4 Matrix4x4::Identity() {
    m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
    m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
    m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
    m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
    return *this;
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
    auto temp = Matrix4x4(1.0f);

    temp.m[1][1] = c;  temp.m[1][2] = s;
    temp.m[2][1] = -s; temp.m[2][2] = c;
    return temp;
}

Matrix4x4 Matrix4x4::RotateY(const float angle) {
    auto s = std::sin(glm::radians(angle));
    auto c = std::cos(glm::radians(angle));
    auto temp = Matrix4x4(1.0f);

    temp.m[0][0] = c;  temp.m[0][2] = -s;
    temp.m[2][0] = s;  temp.m[2][2] = c;
    return temp;
}

Matrix4x4 Matrix4x4::RotateZ(const float angle) {
    auto s = std::sin(glm::radians(angle));
    auto c = std::cos(glm::radians(angle));
    auto temp = Matrix4x4(1.0f);

    temp.m[0][0] = c;  temp.m[0][1] = s;
    temp.m[1][0] = -s; temp.m[1][1] = c;
    return temp;
}

Matrix4x4 Matrix4x4::Transform(const Vector3 translation) {
    auto result = Matrix4x4(1.0f);
    // ????????? ??????? ?????? ?????????? ????????
    result.m[3][0] = translation.x;
    result.m[3][1] = translation.y;
    result.m[3][2] = translation.z;
    return result;
}

Matrix4x4 Matrix4x4::Scale(const Vector3 scale) {
    auto result = Matrix4x4(1.0f);
    result.m[0][0] = scale.x;
    result.m[1][1] = scale.y;
    result.m[2][2] = scale.z;
    return result;
}


Matrix4x4 Matrix4x4::operator*(const Matrix4x4 &m1) const {
    auto temp = Matrix4x4(0.0f);
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
    auto temp = Matrix4x4(0.0f);
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
        return Matrix4x4(1.0f);
    }

    Matrix4x4 result = Matrix4x4(0.0f);
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

// //
// // Created by zykov on 3/15/2026.
// //
//
// #include "Matrix4x4.h"
//
// #include <iostream>
//
// #include "Matrix3x3.h"
// #include "Vector4.h"
//
// Matrix4x4::Matrix4x4() {
//     m[0][0] = 0;
//     m[1][0] = 0;
//     m[2][0] = 0;
//     m[3][0] = 0;
//     m[0][1] = 0;
//     m[1][1] = 0;
//     m[2][1] = 0;
//     m[3][1] = 0;
//     m[0][2] = 0;
//     m[1][2] = 0;
//     m[2][2] = 0;
//     m[3][2] = 0;
//     m[0][3] = 0;
//     m[1][3] = 0;
//     m[2][3] = 0;
//     m[3][3] = 0;
// }
//
// Matrix4x4::Matrix4x4(const float value) : Matrix4x4() {
//     if (value == 1) {
//         //identity
//         Identity();
//     }
// }
//
// Matrix4x4 Matrix4x4::Identity() {
//     m[0][0] = 1;
//     m[1][1] = 1;
//     m[2][2] = 1;
//     m[3][3] = 1;
//     return *this;
// }
//
// Vector4 Matrix4x4::GetColumn(const int column) const {
//     return Vector4(m[column][0], m[column][1], m[column][2], m[column][3]);
// }
//
// Matrix4x4 Matrix4x4::Rotate(const Vector3 axis) {
//     return RotateZ(axis.z) * RotateY(axis.y) * RotateX(axis.x);
// }
//
// Matrix4x4 Matrix4x4::RotateX(const float angle) {
//     auto s = std::sin(glm::radians(angle));
//     auto c = std::cos(glm::radians(angle));
//
//     auto temp = Matrix4x4(1);
//
//     temp.m[1][1] = c;
//     temp.m[1][2] = s;
//     temp.m[2][1] = -s;
//     temp.m[2][2] = c;
//
//     return temp;
// }
//
// Matrix4x4 Matrix4x4::RotateY(const float angle) {
//     auto s = std::sin(glm::radians(angle));
//     auto c = std::cos(glm::radians(angle));
//
//     auto temp = Matrix4x4(1);
//
//     temp.m[0][0] = c;
//     temp.m[2][0] = s;
//     temp.m[0][2] = -s;
//     temp.m[2][2] = c;
//
//     return temp;
// }
//
// Matrix4x4 Matrix4x4::RotateZ(const float angle) {
//     auto s = std::sin(glm::radians(angle));
//     auto c = std::cos(glm::radians(angle));
//
//     auto temp = Matrix4x4(1);
//
//     temp.m[0][0] = c;
//     temp.m[1][0] = -s;
//     temp.m[0][1] = s;
//     temp.m[1][1] = c;
//
//     return temp;
// }
//
// Matrix4x4 Matrix4x4::Transform(const Vector3 translation) {
//     auto result = Matrix4x4(1);
//     result.m[3][0] = translation.x;
//     result.m[3][1] = translation.y;
//     result.m[3][2] = translation.z;
//     return result;
// }
//
// Matrix4x4 Matrix4x4::Scale(const Vector3 scale) {
//     auto result = Matrix4x4(1);
//     result.m[0][0] = scale.x;
//     result.m[1][1] = scale.y;
//     result.m[2][2] = scale.z;
//     return result;
// }
//
// Matrix4x4 Matrix4x4::operator*(const Matrix4x4 &m1) const {
//     auto temp = Matrix4x4(0);
//
//     for (int i = 0; i < 4; i++) {
//         Vector4 column = m1.GetColumn(i);
//         for (int j = 0; j < 4; j++) {
//             Vector4 row = (*this)[j];
//             temp.m[j][i] = Vector4::DotProduct(column, row);
//         }
//     }
//
//     return temp;
// }
//
// Vector4 Matrix4x4::operator*(const Vector4 &v1) const {
//     const auto result = Vector4(
//         m[0][0] * v1.x + m[1][0] * v1.y + m[2][0] * v1.z + m[3][0] * v1.w,
//         m[0][1] * v1.x + m[1][1] * v1.y + m[2][1] * v1.z + m[3][1] * v1.w,
//         m[0][2] * v1.x + m[1][2] * v1.y + m[2][2] * v1.z + m[3][2] * v1.w,
//         m[0][3] * v1.x + m[1][3] * v1.y + m[2][3] * v1.z + m[3][3] * v1.w
//         );
//
//     return result;
// }
//
// Vector3 Matrix4x4::operator*(const Vector3 &v1) const {
//     const auto result = Vector4(
//         m[0][0] * v1.x + m[1][0] * v1.y + m[2][0] * v1.z + m[3][0],
//         m[0][1] * v1.x + m[1][1] * v1.y + m[2][1] * v1.z + m[3][1],
//         m[0][2] * v1.x + m[1][2] * v1.y + m[2][2] * v1.z + m[3][2],
//         m[0][3] * v1.x + m[1][3] * v1.y + m[2][3] * v1.z + m[3][3]
//         );
//
//     return Vector3(result.x / result.w, result.y / result.w, result.z / result.w);
// }
//
// Matrix4x4 Matrix4x4::Transpose() const {
//     auto temp = Matrix4x4(0);
//
//     for (int i = 0; i < 4; i++) {
//         for (int j = 0; j < 4; j++) {
//             temp.m[i][j] = m[j][i];
//         }
//     }
//
//     return temp;
// }
//
// Matrix4x4 Matrix4x4::Inverse() const {
//     Matrix4x4 result = Matrix4x4(0);
//
//     for (int c = 0; c < 4; c++) {
//         for (int r = 0; r < 4; r++) {
//             if ((c + r) % 2 == 0) {
//                 result.m[c][r] = Minor(c, r).Determinant();
//             } else {
//                 result.m[c][r] = -Minor(c, r).Determinant();
//             }
//         }
//     }
//
//     result = result.Transpose();
//
//     for (int c = 0; c < 4; c++) {
//         for (int r = 0; r < 4; r++) {
//             result.m[c][r] /= Determinant();
//         }
//     }
//
//     return result;
// }
//
// Matrix3x3 Matrix4x4::Minor(const int column, const int row) const {
//     Matrix3x3 result;
//
//     for (int i = 0; i < 4; i++) {
//         for (int j = 0; j < 4; j++) {
//             //exclude passed column and row
//             if (i != column && j != row) {
//                 //if current column > column then we need offset indexes
//                 if (i > column) {
//                     //if current row > row then we need offset indexes
//                     if (j > row) {
//                         result.m[i-1][j-1] = m[i][j];
//                     } else {
//                         result.m[i-1][j] = m[i][j];
//                     }
//                 } else {
//                     if (j > row) {
//                         result.m[i][j-1] = m[i][j];
//                     } else {
//                         result.m[i][j] = m[i][j];
//                     }
//                 }
//             }
//         }
//     }
//
//     return result;
// }
//
// float Matrix4x4::Determinant() const {
//     float det = 0;
//
//     for (int k = 0; k < 4; k++) {
//         Matrix3x3 result = Minor(k, 0);
//
//         if (k % 2 == 0) {
//             det += result.Determinant() * m[k][0];
//         } else {
//             det -= result.Determinant() * m[k][0];
//         }
//     }
//
//     return det;
// }
//
// Vector4 Matrix4x4::operator[](const unsigned int row) const {
//     return Vector4(m[0][row], m[1][row], m[2][row], m[3][row]);
// }