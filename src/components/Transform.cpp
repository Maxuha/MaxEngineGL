#include "Transform.h"
#include <iostream>
#include "../math/Matrix4x4.h"

Vector3 Transform::Forward() const {
    const Matrix4x4 rotMat = rotation.ToMatrix4x4();
    return Vector3{rotMat.m[2][0], rotMat.m[2][1], rotMat.m[2][2]}.Normalize();
}

Vector3 Transform::Up() const {
    const Matrix4x4 rotMat = rotation.ToMatrix4x4();
    return Vector3{rotMat.m[1][0], rotMat.m[1][1], rotMat.m[1][2]}.Normalize();
}

Vector3 Transform::Right() const {
    const Matrix4x4 rotMat = rotation.ToMatrix4x4();
    return Vector3{rotMat.m[0][0], rotMat.m[0][1], rotMat.m[0][2]}.Normalize();
}

Matrix4x4 Transform::GetLocalMatrix() const {
    const Matrix4x4 rotMat = rotation.Normalized().ToMatrix4x4();
    const auto model = Matrix4x4::Transform(position) * rotMat * Matrix4x4::Scale(scale);
    return model;
}

Matrix4x4 Transform::GetWorldMatrix() const {
    auto model = GetLocalMatrix();

    if (parent != nullptr) {
        model = parent->GetWorldMatrix() * model;
    }

    return model;
}

Matrix4x4 Transform::LookAt() const {
    const auto viewMatrix = GetWorldMatrix();
    return viewMatrix.Inverse();
    return Matrix4x4::LookAt(position, position + Forward(), Up());
}

Matrix4x4 Transform::Perspective(const float fov, const float aspectRatio, const float zNear, const float zFar) const {
    return Matrix4x4::Perspective(fov, aspectRatio, zNear, zFar);
}

Matrix4x4 Transform::Othographic(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar) const {
    return Matrix4x4::Orthographic(left, right, bottom, top, zNear, zFar);
}

void Transform::SetParent(Transform *parent) {

    auto localMatrix = GetLocalMatrix();

    if (parent == nullptr) {
        localMatrix = GetWorldMatrix();
    } else {
        localMatrix = parent->GetWorldMatrix().Inverse() * GetWorldMatrix();
    }

    position.x = localMatrix.m[3][0];
    position.y = localMatrix.m[3][1];
    position.z = localMatrix.m[3][2];

    this->parent = parent;
}

void Transform::AddChild(Transform *child) {
    children.push_back(child);
}

void Transform::RemoveChild(Transform *child) {
    std::erase(children, child);
}

Transform* Transform::GetChild(const int index) const {
    return children[index];
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
    // move to camera logic
    //rotation.x = std::clamp(rotation.x, -90.0f, 90.0f);
}

void Transform::RotateRoll(const float angle) {
    rotation.z += angle;
}

std::ostream & operator<<(std::ostream &os, const Transform &transform) {
    os << "Position: (" << transform.position << ")" << "\n";
    os << "Rotation: (" << transform.rotation.x << ";" << transform.rotation.y << ";" << transform.rotation.z << ")" << "\n";
    os << "Scale: (" << transform.scale << ")" << "\n";
    return os;
}
