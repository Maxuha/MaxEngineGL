//
// Created by zykov on 4/9/2026.
//

#ifndef MAXENGINE_VECTOR4_H
#define MAXENGINE_VECTOR4_H
#include "Vector3.h"


class Vector4 {
public:
    float x, y, z, w;

    Vector4();

    Vector4(const float x, const float y, const float z, const float w) : x(x), y(y), z(z), w(w) {}

    Vector4(const Vector3 v, const float w) : x(v.x), y(v.y), z(v.z), w(w) {}

    static float DotProduct(const Vector4 &v1, const Vector4 &v2);

    Vector4 operator*(const float value) const;
};



#endif //MAXENGINE_VECTOR4_H
