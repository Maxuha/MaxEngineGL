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

    auto Position = transform->position;
    auto Forward = transform->Forward();
    auto Right = transform->Right();
    auto Up = transform->Up();

    auto viewMatrix = Matrix4x4(1);

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

    return viewMatrix;
}

Matrix4x4 Camera::ProjectionMatrix() const {
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

Matrix4x4 Camera::ViewProjectionMatrix() {
    return ViewMatrix() * ProjectionMatrix();
    // return ProjectionMatrix() * ViewMatrix();
}
