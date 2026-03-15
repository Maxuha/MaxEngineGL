//
// Created by zykov on 3/15/2026.
//

#ifndef MAXENGINE_VECTOR_H
#define MAXENGINE_VECTOR_H


class Vector3 {
public:
    float x, y, z;

    static Vector3 Zero();

    static Vector3 Up();

    static Vector3 Right();

    static Vector3 Forward();

    Vector3 operator-(const Vector3 &v) const;

    Vector3 operator+(const Vector3 &v) const;

    Vector3 operator*(float value) const;

    Vector3 operator/(float value) const;

    Vector3 operator+=(const Vector3 &v);

    Vector3 operator-=(const Vector3 &v);

    Vector3 operator*=(const Vector3 &v);

    Vector3 operator/=(const Vector3 &v);

    float Length(const Vector3 &v) const;

    Vector3 Normalize();

    static Vector3 CrossProduct(const Vector3 &a, const Vector3 &b);

    static float DotProduct(const Vector3 &a, const Vector3 &b);
};


#endif //MAXENGINE_VECTOR_H