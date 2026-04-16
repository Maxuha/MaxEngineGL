//
// Created by zykov on 3/15/2026.
//

#ifndef MAXENGINE_TRANSFORM_H
#define MAXENGINE_TRANSFORM_H
#include <iostream>
#include "Component.h"
#include "../math/Matrix4x4.h"
#include "../math/Vector3.h"

class Transform : public Component {
    public:
    explicit Transform(GameObject* gameObject) : Component(gameObject) {}

    Vector3 position = Vector3::Zero();
    Vector3 rotation = Vector3::Zero();
    Vector3 scale = Vector3::One();

    Transform* parent = nullptr;

    Vector3 Forward() const;
    Vector3 Up() const;
    Vector3 Right() const;

    Matrix4x4 GetLocalMatrix() const;
    Matrix4x4 GetWorldMatrix() const;

    Vector3 pivot = { 0.5f, 0.5f, 0.5f };

    void SetParent(Transform* parent);

    void Translate(Vector3 dir);

    void RotateYaw(float angle);
    void RotatePitch(float angle);
    void RotateRoll(float angle);

    friend std::ostream& operator<<(std::ostream& os, const Transform& transform);
};


#endif //MAXENGINE_TRANSFORM_H