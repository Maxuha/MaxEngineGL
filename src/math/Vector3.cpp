//
// Created by zykov on 3/15/2026.
//

#include "Vector3.h"
#include <cmath>

Vector3 Vector3::Zero() {
    return Vector3(0.0f, 0.0f, 0.0f);
}

Vector3 Vector3::One() {
    return Vector3(1.0f, 1.0f, 1.0f);
}

Vector3 Vector3::Up() {
    return Vector3(0.0f, 1.0f, 0.0f);
}

Vector3 Vector3::Right() {
    return Vector3(1.0f, 0.0f, 0.0f);
}

Vector3 Vector3::Forward() {
    return Vector3(0.0f, 0.0f, 1.0f);
}

Vector3 Vector3::operator-() const {
    return Vector3(-x, -y, -z);
}

Vector3 Vector3::operator-(const Vector3 &v) const {
    return Vector3(x - v.x, y - v.y, z - v.z);
}

Vector3 Vector3::operator+(const Vector3 &v) const {
    return Vector3(x + v.x, y + v.y, z + v.z);
}

Vector3 Vector3::operator*(const float value) const {
    return Vector3(x * value, y * value, z * value);
}

Vector3 Vector3::operator/(const float value) const {
    return Vector3(x / value, y / value, z / value);
}

Vector3 Vector3::operator+=(const Vector3 &v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

Vector3 Vector3::operator-=(const Vector3 &v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

Vector3 Vector3::operator*=(const Vector3 &v) {
    x *= v.x;
    y *= v.y;
    z *= v.z;
    return *this;
}

Vector3 Vector3::operator/=(const Vector3 &v) {
    x /= v.x;
    y /= v.y;
    z /= v.z;
    return *this;
}

float Vector3::Length(const Vector3 &v) const {
    return std::sqrt(x * x + y * y + z * z);
}

Vector3 Vector3::Normalize() {
    // if length == 0, vector can't be normalized
    if (const float length = Length(*this); length != 0.0f) {
        x /= length;
        y /= length;
        z /= length;
    }

    return *this;
}

Vector3 Vector3::CrossProduct(const Vector3 &a, const Vector3 &b) {
    return Vector3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

float Vector3::DotProduct(const Vector3 &a, const Vector3 &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
