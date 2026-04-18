#include "Transform.h"
#include <iostream>
#include "../math/Matrix4x4.h"

Vector3 Transform::Forward() const {
    const Matrix4x4 rotMat = Matrix4x4::Rotate(rotation);
    return Vector3(rotMat.m[2][0], rotMat.m[2][1], rotMat.m[2][2]);
}

Vector3 Transform::Up() const {
    const Matrix4x4 rotMat = Matrix4x4::Rotate(rotation);
    return Vector3(rotMat.m[1][0], rotMat.m[1][1], rotMat.m[1][2]);
}

Vector3 Transform::Right() const {
    const Matrix4x4 rotMat = Matrix4x4::Rotate(rotation);
    return Vector3(rotMat.m[0][0], rotMat.m[0][1], rotMat.m[0][2]);
}

Matrix4x4 Transform::GetLocalMatrix() const {
    const auto model = Matrix4x4::Transform(position) * Matrix4x4::Rotate(rotation) * Matrix4x4::Scale(scale);
    return model;
}

Matrix4x4 Transform::GetWorldMatrix() const {
    auto model = GetLocalMatrix();

    if (parent != nullptr) {
        model = model * parent->GetWorldMatrix();
    }

    return model;
}

void Transform::SetParent(Transform *parent) {

    auto localMatrix = Matrix4x4(0);

    if (parent == nullptr) {
        localMatrix = GetLocalMatrix() * this->parent->GetWorldMatrix();
    } else {
        localMatrix = GetLocalMatrix() * parent->GetWorldMatrix().Inverse();
    }

    position.x = localMatrix.m[3][0];
    position.y = localMatrix.m[3][1];
    position.z = localMatrix.m[3][2];

    this->parent = parent;
}

void Transform::Translate(const Vector3 dir) {
    position += dir;
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

std::ostream & operator<<(std::ostream &os, const Transform &transform) {
    os << "Position: (" << transform.position.x << ";" << transform.position.y << ";" << transform.position.z << ")" << "\n";
    os << "Rotation: (" << transform.rotation.x << ";" << transform.rotation.y << ";" << transform.rotation.z << ")" << "\n";
    os << "Scale: (" << transform.scale.x << ";" << transform.scale.y << ";" << transform.scale.z << ")" << "\n";
    return os;
}
