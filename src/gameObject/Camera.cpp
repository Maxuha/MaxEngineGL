//
// Created by zykov on 3/15/2026.
//

#include "Camera.h"
#include <cmath>
#include <iostream>
#include "../components/Transform.h"
#include "../math/Matrix4x4.h"
#include "glm/ext/matrix_clip_space.hpp"

Matrix4x4 Camera::ViewMatrix() {
    auto transform = GetComponent<Transform>();

    auto Position = transform->position();
    auto Forward = transform->forward();
    auto Right = transform->right();
    auto Up = transform->up();

    Matrix4x4 viewMatrix = Matrix4x4(1);

    viewMatrix.m[0][0] = Right.x;
    viewMatrix.m[1][0] = Right.y;
    viewMatrix.m[2][0] = Right.z;

    viewMatrix.m[0][1] = Up.x;
    viewMatrix.m[1][1] = Up.y;
    viewMatrix.m[2][1] = Up.z;

    viewMatrix.m[0][2] = -Forward.x;
    viewMatrix.m[1][2] = -Forward.y;
    viewMatrix.m[2][2] = -Forward.z;

    viewMatrix.m[3][0] = -Vector3::DotProduct(Right, Position);
    viewMatrix.m[3][1] = -Vector3::DotProduct(Up, Position);
    viewMatrix.m[3][2] = Vector3::DotProduct(Forward, Position);
    viewMatrix.m[3][3] = 1;

    return viewMatrix;
}

Matrix4x4 Camera::ProjectionMatrix() const {
    Matrix4x4 projectionMatrix = Matrix4x4(0);

    float fovRad = glm::radians(fov);
    float tanHalfFov = std::tan(fovRad / 2);

    projectionMatrix.m[0][0] = 1 / (aspectRatio * tanHalfFov);
    projectionMatrix.m[1][1] = 1 / tanHalfFov;
    projectionMatrix.m[2][2] = - (far + near) / (far - near);
    projectionMatrix.m[2][3] = -1;
    projectionMatrix.m[3][2] = - (2 * far * near) / (far - near);

    return projectionMatrix;
}

glm::mat4 Camera::ViewProjectionMatrix() {
    return {};
    // return ProjectionMatrix() * ViewMatrix();
}
