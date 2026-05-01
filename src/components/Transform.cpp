#include "Transform.h"
#include <iostream>
#include "../math/Matrix4x4.h"

Vector3 Transform::Forward() const {
    const Matrix4x4 rotMat = Matrix4x4::Rotate(rotation);
    return {rotMat.m[2][0], rotMat.m[2][1], rotMat.m[2][2]};
}

Vector3 Transform::Up() const {
    const Matrix4x4 rotMat = Matrix4x4::Rotate(rotation);
    return {rotMat.m[1][0], rotMat.m[1][1], rotMat.m[1][2]};
}

Vector3 Transform::Right() const {
    const Matrix4x4 rotMat = Matrix4x4::Rotate(rotation);
    return {rotMat.m[0][0], rotMat.m[0][1], rotMat.m[0][2]};
}

Matrix4x4 Transform::GetLocalMatrix() const {
    const auto model = Matrix4x4::Transform(position) * Matrix4x4::Rotate(rotation) * Matrix4x4::Scale(scale);
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
    auto viewMatrix = GetWorldMatrix();

    // Invert z for OpenGL
    viewMatrix.m[2][0] = -viewMatrix.m[2][0];
    viewMatrix.m[2][1] = -viewMatrix.m[2][1];
    viewMatrix.m[2][2] = -viewMatrix.m[2][2];
    viewMatrix.m[2][3] = -viewMatrix.m[2][3];

    viewMatrix = viewMatrix.Inverse();

    return viewMatrix;
}

Matrix4x4 Transform::Perspective(const float fov, const float aspectRatio, const float near, const float far) const {
    auto projectionMatrix = Matrix4x4(0);

    const float fovRad = glm::radians(fov);
    const float tanHalfFov = std::tan(fovRad / 2);

    projectionMatrix.m[0][0] = 1 / tanHalfFov / aspectRatio;
    projectionMatrix.m[1][1] = 1 / tanHalfFov;
    projectionMatrix.m[2][2] = -far / (far - near) ;
    projectionMatrix.m[3][2] = -(near * far) / (far - near);
    projectionMatrix.m[2][3] = -1;

    return projectionMatrix;
}

Matrix4x4 Transform::Othographic(float left, float right, float bottom, float top, float near, float far) const {
    auto projectionMatrix = Matrix4x4(0);
    projectionMatrix.m[0][0] = 2 / (right - left);
    projectionMatrix.m[1][1] = 2 / (top - bottom);
    projectionMatrix.m[2][2] = -2 / (far - near);
    projectionMatrix.m[3][0] = -(right + left) / (right - left);
    projectionMatrix.m[3][1] = -(top + bottom) / (top - bottom);
    projectionMatrix.m[3][2] = -(far + near) / (far - near);
    projectionMatrix.m[3][3] = 1;
    return projectionMatrix;
}

void Transform::SetParent(Transform *parent) {

    auto localMatrix = GetLocalMatrix();

    if (parent == nullptr) {
       // localMatrix = GetWorldMatrix() * this->parent->GetWorldMatrix();
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
    os << "Position: (" << transform.position.x << ";" << transform.position.y << ";" << transform.position.z << ")" << "\n";
    os << "Rotation: (" << transform.rotation.x << ";" << transform.rotation.y << ";" << transform.rotation.z << ")" << "\n";
    os << "Scale: (" << transform.scale.x << ";" << transform.scale.y << ";" << transform.scale.z << ")" << "\n";
    return os;
}
