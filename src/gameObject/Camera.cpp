//
// Created by zykov on 3/15/2026.
//

#include "Camera.h"

#include "../di/DIContainer.h"


Camera::Camera(const float fov, const float zNear, const float zFar, const float aspectRatio) : fov(fov), zNear(zNear),
                                                                                              zFar(zFar),
                                                                                              aspectRatio(aspectRatio) {
    projection = GetTransform()->Perspective(fov, aspectRatio, zNear, zFar);

    handle = DIContainer::GetInstance().Get<Rendering::IRenderer>()->CreateCamera(fov, zNear, zFar, aspectRatio)->handle;
}

Matrix4x4 Camera::GetProjection() const {
    return projection;
}

Rendering::CameraHandle Camera::GetHandle() const {
    return handle;
}
