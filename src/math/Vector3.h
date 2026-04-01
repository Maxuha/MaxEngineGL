//
// Created by zykov on 3/15/2026.
//

#ifndef MAXENGINE_VECTOR_H
#define MAXENGINE_VECTOR_H

#include <glm/glm.hpp>

class Vector3 {
public:
    float x, y, z;

    static Vector3 Zero();

    static Vector3 One();

    static Vector3 Up();

    static Vector3 Right();

    static Vector3 Forward();

    Vector3 operator-() const;

    Vector3 operator-(const Vector3 &v) const;

    Vector3 operator+(const Vector3 &v) const;

    Vector3 operator*(float value) const;

    Vector3 operator/(float value) const;

    Vector3 operator+=(const Vector3 &v);

    Vector3 operator-=(const Vector3 &v);

    Vector3 operator*=(const Vector3 &v);

    Vector3 operator/=(const Vector3 &v);

    friend bool operator==(const Vector3 &v1, const Vector3 &v2) {
        return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
    }

    float Length(const Vector3 &v) const;

    Vector3 Normalize();

    static Vector3 CrossProduct(const Vector3 &a, const Vector3 &b);

    static float DotProduct(const Vector3 &a, const Vector3 &b);

    template <typename T>
    T Convert() {
        auto result = glm::vec3();
        result.x = x;
        result.y = y;
        result.z = z;
        return result;
    }
};


#endif //MAXENGINE_VECTOR_H