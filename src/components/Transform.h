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
    std::vector<Transform*> children;

    Vector3 Forward() const;
    Vector3 Up() const;
    Vector3 Right() const;

    Matrix4x4 GetLocalMatrix() const;
    Matrix4x4 GetWorldMatrix() const;
    Matrix4x4 LookAt() const;
    Matrix4x4 Perspective(float fov, float aspectRatio, float near, float far) const;
    Matrix4x4 Othographic(float left, float right, float bottom, float top, float near, float far) const;

    Vector3 pivot = { 0.5f, 0.5f, 0.5f };

    void SetParent(Transform* parent);
    void AddChild(Transform* child);
    void RemoveChild(Transform* child);
    Transform* GetChild(int index) const;

    void Translate(Vector3 dir);

    void RotateYaw(float angle);
    void RotatePitch(float angle);
    void RotateRoll(float angle);

    friend std::ostream& operator<<(std::ostream& os, const Transform& transform);
};


#endif //MAXENGINE_TRANSFORM_H