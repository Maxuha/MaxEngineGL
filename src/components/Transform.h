//
// Created by zykov on 3/15/2026.
//

#ifndef MAXENGINE_TRANSFORM_H
#define MAXENGINE_TRANSFORM_H
#include "Component.h"
#include "../math/Matrix4x4.h"
#include "../math/Vector3.h"

class Transform : public Component {
    public:
    Vector3 position = Vector3::Zero();
    Vector3 rotation = Vector3::Zero();
    Vector3 scale = Vector3{5, 5,5};

    Vector3 Forward() const;
    Vector3 Up() const;
    Vector3 Right() const;

    Matrix4x4 GetModelMatrix() const;

    Vector3 pivot = { 0.5f, 0.5f, 0.5f };

    void Translate(Vector3 dir);

    void RotateYaw(float angle);
    void RotatePitch(float angle);
    void RotateRoll(float angle);
};


#endif //MAXENGINE_TRANSFORM_H