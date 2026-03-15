//
// Created by zykov on 3/15/2026.
//

#include "Camera.h"

#include "Transform.h"
#include "glm/ext/matrix_clip_space.hpp"

glm::mat4 Camera::ViewMatrix() {
    auto transform = GetComponent<Transform>();
    auto Position = transform->position;
    auto Forward = transform->forward;
    auto Up = transform->up;

    const glm::vec3 cameraPos = {Position.x, Position.y, Position.z};
    const glm::vec3 cameraAt = {Forward.x, Forward.y, Forward.z};
    const glm::vec3 cameraUp = {Up.x, Up.y, Up.z};

    return glm::lookAt(cameraPos, cameraPos + cameraAt, cameraUp);
}

glm::mat4 Camera::ProjectionMatrix() const {
    return glm::perspective(glm::radians(fov), aspectRatio, near, far);
}

glm::mat4 Camera::ViewProjectionMatrix() {
    return glm::perspective(glm::radians(fov), aspectRatio, near, far) * ViewMatrix();
}
