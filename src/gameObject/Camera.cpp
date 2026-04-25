//
// Created by zykov on 3/15/2026.
//

#include "Camera.h"


Camera::Camera(const float fov, const float near, const float far, const float aspectRatio) : fov(fov), near(near),
    far(far),
    aspectRatio(aspectRatio) {
    projection = GetTransform()->Perspective(fov, aspectRatio, near, far);
}

Matrix4x4 Camera::GetProjection() const {
    return GetTransform()->Perspective(fov, aspectRatio, near, far);
}
