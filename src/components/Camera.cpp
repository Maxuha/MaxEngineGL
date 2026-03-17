//
// Created by zykov on 3/15/2026.
//

#include "Camera.h"

#include <iostream>

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

    std::cout << "Position: " << Position.x << " " << Position.y << " " << Position.z << " " << std::endl;
    std::cout << "Up: " << Up.x << " " << Up.y << " " << Up.z << " " << std::endl;
    std::cout << "Forward: " << Forward.x << " " << Forward.y << " " << Forward.z << " " << std::endl;
    std::cout << "Right: " << transform->Right().x << " " << transform->Right().y << " " << transform->Right().z << " " << std::endl;

    return glm::lookAt(cameraPos, cameraPos + cameraAt, cameraUp);
}

glm::mat4 Camera::ProjectionMatrix() const {
    return glm::perspective(glm::radians(fov), aspectRatio, near, far);
}

glm::mat4 Camera::ViewProjectionMatrix() {
    return ProjectionMatrix() * ViewMatrix();
}
