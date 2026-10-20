//
// Created by zykov on 10/18/2026.
//

#ifndef MAXENGINE_QUATERNION_H
#define MAXENGINE_QUATERNION_H


class Quaternion {
public:
    float x, y, z, w;

    Quaternion();
    Quaternion(float x, float y, float z, float w);

    static Quaternion FromAxisAngle(float ax, float ay, float az, float angleRadians);

    // ?????????
    Quaternion operator+(const Quaternion& q) const;
    Quaternion operator*(const Quaternion& q) const;
    Quaternion operator*(float scalar) const;

    float Length() const;
    Quaternion Normalized() const;
    Quaternion Conjugate() const;
    float Dot(const Quaternion& q) const;

    static Quaternion Slerp(const Quaternion& start, const Quaternion& end, float t);

    void ToMatrix4x4(float* m) const;
};


#endif //MAXENGINE_QUATERNION_H
