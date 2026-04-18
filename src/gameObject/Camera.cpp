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
    const auto transform = GetComponent<Transform>();

    auto viewMatrix = transform->GetWorldMatrix();

    // Invert z for OpenGL
    viewMatrix.m[2][0] = -viewMatrix.m[2][0];
    viewMatrix.m[2][1] = -viewMatrix.m[2][1];
    viewMatrix.m[2][2] = -viewMatrix.m[2][2];
    viewMatrix.m[2][3] = -viewMatrix.m[2][3];

    viewMatrix = viewMatrix.Inverse();

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
