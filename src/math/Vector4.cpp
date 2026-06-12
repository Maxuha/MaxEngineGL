//
// Created by zykov on 4/9/2026.
//

#include "Vector4.h"

Vector4::Vector4() {
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
    w = 0.0f;
}

float Vector4::DotProduct(const Vector4 &v1, const Vector4 &v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

Vector4 Vector4::operator*(const float value) const {
    return Vector4(x * value, y * value, z * value, w * value);
}
