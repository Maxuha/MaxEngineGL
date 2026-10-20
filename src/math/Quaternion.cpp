//
// Created by zykov on 10/18/2026.
//

#include "Quaternion.h"

Quaternion::Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}

Quaternion::Quaternion(const float x, const float y, const float z, const float w) : x(x), y(y), z(z), w(w) {}

Quaternion Quaternion::FromAxisAngle(float ax, float ay, float az, float angleRadians) {
    float halfAngle = angleRadians * 0.5f;
    float sinHalf = std::sin(halfAngle);
    return Quaternion(ax * sinHalf, ay * sinHalf, az * sinHalf, std::cos(halfAngle));
}

Quaternion Quaternion::operator+(const Quaternion& q) const {
    return Quaternion(x + q.x, y + q.y, z + q.z, w + q.w);
}

Quaternion Quaternion::operator*(const Quaternion& q) const {
    return Quaternion(
        w * q.x + x * q.w + y * q.z - z * q.y,
        w * q.y + y * q.w + z * q.x - x * q.z,
        w * q.z + z * q.w + x * q.y - y * q.x,
        w * q.w - x * q.x - y * q.y - z * q.z
    );
}

Quaternion Quaternion::operator*(float scalar) const {
    return Quaternion(x * scalar, y * scalar, z * scalar, w * scalar);
}

float Quaternion::Length() const {
    return std::sqrt(x * x + y * y + z * z + w * w);
}

Quaternion Quaternion::Normalized() const {
    float len = Length();
    if (len == 0.0f) return Quaternion();
    float invLen = 1.0f / len;
    return Quaternion(x * invLen, y * invLen, z * invLen, w * invLen);
}

Quaternion Quaternion::Conjugate() const {
    return Quaternion(-x, -y, -z, w);
}

float Quaternion::Dot(const Quaternion& q) const {
    return x * q.x + y * q.y + z * q.z + w * q.w;
}

Quaternion Quaternion::Slerp(const Quaternion& start, const Quaternion& end, const float t) {
    Quaternion q1 = start.Normalized();
    Quaternion q2 = end.Normalized();

    float dot = q1.Dot(q2);

    if (dot < 0.0f) {
        q2 = Quaternion(-q2.x, -q2.y, -q2.z, -q2.w);
        dot = -dot;
    }

    if (dot > 0.9995f) {
        return Quaternion(
            q1.x + (q2.x - q1.x) * t,
            q1.y + (q2.y - q1.y) * t,
            q1.z + (q2.z - q1.z) * t,
            q1.w + (q2.w - q1.w) * t
        ).Normalized();
    }

    float theta_0 = std::acos(dot);
    float theta = theta_0 * t;
    float sin_theta = std::sin(theta);
    float sin_theta_0 = std::sin(theta_0);

    float s0 = std::cos(theta) - dot * sin_theta / sin_theta_0;
    float s1 = sin_theta / sin_theta_0;

    return (q1 * s0) + (q2 * s1);
}

void Quaternion::ToMatrix4x4(float* m) const {
    float xx = x * x; float yy = y * y; float zz = z * z;
    float xy = x * y; float xz = x * z; float yz = y * z;
    float wx = w * x; float wy = w * y; float wz = w * z;

    m[0]  = 1.0f - 2.0f * (yy + zz); m[1]  = 2.0f * (xy - wz);        m[2]  = 2.0f * (xz + wy);        m[3]  = 0.0f;
    m[4]  = 2.0f * (xy + wz);        m[5]  = 1.0f - 2.0f * (xx + zz); m[6]  = 2.0f * (yz - wx);        m[7]  = 0.0f;
    m[8]  = 2.0f * (xz - wy);        m[9]  = 2.0f * (yz + wx);        m[10] = 1.0f - 2.0f * (xx + yy); m[11] = 0.0f;
    m[12] = 0.0f;                    m[13] = 0.0f;                    m[14] = 0.0f;                    m[15] = 1.0f;
}
