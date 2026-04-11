#include "Transform.h"

#include <iostream>

#include "../math/Matrix4x4.h"

Vector3 Transform::Forward() const {
    const Matrix4x4 rotMat = Matrix4x4::RotateZ(rotation.z) * Matrix4x4::RotateY(rotation.y) * Matrix4x4::RotateX(rotation.x);
    return Vector3(rotMat.m[0][2], rotMat.m[1][2], rotMat.m[2][2]);
}

Vector3 Transform::Up() const {
    const Matrix4x4 rotMat = Matrix4x4::RotateZ(rotation.z) * Matrix4x4::RotateY(rotation.y) * Matrix4x4::RotateX(rotation.x);
    return Vector3(rotMat.m[0][1], rotMat.m[1][1], rotMat.m[2][1]);
}

Vector3 Transform::Right() const {
    const Matrix4x4 rotMat = Matrix4x4::RotateZ(rotation.z) * Matrix4x4::RotateY(rotation.y) * Matrix4x4::RotateX(rotation.x);
    return Vector3(rotMat.m[0][0], rotMat.m[1][0], rotMat.m[2][0]);
}

Matrix4x4 Transform::GetLocalMatrix() const {
    const auto model = Matrix4x4::Transform(-pivot) * Matrix4x4::Scale(scale) * Matrix4x4::Rotate(rotation) * Matrix4x4::Transform(pivot) * Matrix4x4::Transform(position);
    return model;
}

Matrix4x4 Transform::GetModelMatrix() const {
    auto model = GetLocalMatrix();

    if (parent != nullptr) {
        model = model * parent->GetModelMatrix();
    }

    return model;
}

void Transform::SetParent(Transform *parent) {
    std::cout << "GlobalPos:" << position.x << " " << position.y << " " << position.z << std::endl;

    auto localMatrix = Matrix4x4(0);

    if (parent == nullptr) {
        localMatrix = GetLocalMatrix() * this->parent->GetModelMatrix();
    } else {
        localMatrix = GetLocalMatrix() * parent->GetModelMatrix().Inverse();
    }

    position.x = localMatrix.m[3][0];
    position.y = localMatrix.m[3][1];
    position.z = localMatrix.m[3][2];

    std::cout << "localPos: " << position.x << " " << position.y << " " << position.z << std::endl;
    this->parent = parent;
}

void Transform::Translate(const Vector3 dir) {
    const Vector3 relativeDir = Right() * dir.x + Up() * dir.y + Forward() * dir.z;
    position += relativeDir;
}

void Transform::RotateYaw(const float angle) {
    rotation.y += angle;
}

void Transform::RotatePitch(const float angle) {
    // Pitch angles (-90; 90)
    rotation.x += angle;
    rotation.x = std::clamp(rotation.x, -90.0f, 90.0f);
}

void Transform::RotateRoll(const float angle) {
    rotation.z += angle;
}
