//
// Created by zykov on 10/18/2026.
//

#ifndef MAXENGINE_QUATERNION_H
#define MAXENGINE_QUATERNION_H

constexpr float Pi = 3.14159265358979323846f;

class Quaternion {
public:
    float x, y, z, w;

    Quaternion();
    Quaternion(float x, float y, float z, float w);

    static Quaternion FromAxisAngle(float ax, float ay, float az, float angle);

    static Quaternion FromEulerAngles(float pitch, float yaw, float roll);

    static Quaternion Identity();

    Quaternion operator+(const Quaternion& q) const;
    Quaternion operator*(const Quaternion& q) const;
    Quaternion operator*(float scalar) const;

    float Length() const;
    Quaternion Normalized() const;
    Quaternion Conjugate() const;
    float Dot(const Quaternion& q) const;

    static Quaternion Slerp(const Quaternion& start, const Quaternion& end, float t);

    Matrix4x4 ToMatrix4x4() const;

    friend std::ostream& operator<<(std::ostream& os, const Quaternion& q);
};


#endif //MAXENGINE_QUATERNION_H
