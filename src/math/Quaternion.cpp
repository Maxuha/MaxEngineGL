//
// Created by zykov on 10/18/2026.
//

#include "Quaternion.h"

Quaternion::Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}

Quaternion::Quaternion(const float x, const float y, const float z, const float w) : x(x), y(y), z(z), w(w) {}

Quaternion Quaternion::FromAxisAngle(const float ax, const float ay, const float az, const float angle) {
    const float halfAngle = angle * (Pi / 180) * 0.5f;
    const float sinHalf = std::sin(halfAngle);
    return Quaternion(ax * sinHalf, ay * sinHalf, az * sinHalf, std::cos(halfAngle)).Normalized();
}

Quaternion Quaternion::FromEulerAngles(const float pitch, const float yaw, const float roll) {
    const Quaternion qYaw   = FromAxisAngle(0.0f, 1.0f, 0.0f, yaw);
    const Quaternion qPitch = FromAxisAngle(1.0f, 0.0f, 0.0f, pitch);
    const Quaternion qRoll  = FromAxisAngle(0.0f, 0.0f, 1.0f, roll);

    return (qYaw * qPitch * qRoll).Normalized();
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

Matrix4x4 Quaternion::ToMatrix4x4() const {
    float xx = x * x;   float xy = x * y;   float xz = x * z;
    float yy = y * y;   float yz = y * z;   float zz = z * z;
    float wx = w * x;   float wy = w * y;   float wz = w * z;

    Matrix4x4 m;

    m.m[0][0] = 1.0f - 2.0f * (yy + zz);
    m.m[0][1] = 2.0f * (xy + wz);
    m.m[0][2] = 2.0f * (xz - wy);
    m.m[0][3] = 0.0f;

    m.m[1][0] = 2.0f * (xy - wz);
    m.m[1][1] = 1.0f - 2.0f * (xx + zz);
    m.m[1][2] = 2.0f * (yz + wx);
    m.m[1][3] = 0.0f;

    m.m[2][0] = 2.0f * (xz + wy);
    m.m[2][1] = 2.0f * (yz - wx);
    m.m[2][2] = 1.0f - 2.0f * (xx + yy);
    m.m[2][3] = 0.0f;

    m.m[3][0] = 0.0f;
    m.m[3][1] = 0.0f;
    m.m[3][2] = 0.0f;
    m.m[3][3] = 1.0f;

    return m;
}

Quaternion Quaternion::Identity() {
    return Quaternion(0, 0, 0, 1);
}

std::ostream & operator<<(std::ostream &os, const Quaternion &q) {
    os << "Quaternion(" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << ")";
    return os;
}
