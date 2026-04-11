//
// Created by zykov on 4/10/2026.
//

#include "Matrix3x3.h"

Matrix3x3::Matrix3x3() {
    m[0][0] = 0;
    m[0][1] = 0;
    m[0][2] = 0;
    m[1][0] = 0;
    m[1][1] = 0;
    m[1][2] = 0;
    m[2][0] = 0;
    m[2][1] = 0;
    m[2][2] = 0;
}

float Matrix3x3::Determinant() {
    const float det = m[0][0] * m[1][1] * m[2][2] +
                m[1][0] * m[2][1] * m[0][2] +
                m[2][0] * m[0][1] * m[1][2] -
                m[2][0] * m[1][1] * m[0][2] -
                m[1][0] * m[0][1] * m[2][2] -
                m[0][0] * m[2][1] * m[1][2];
    return det;
}
