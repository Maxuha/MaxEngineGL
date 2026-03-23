//
// Created by zykov on 3/15/2026.
//

#include "Transform.h"

#include "glm/fwd.hpp"
#include "glm/ext/matrix_transform.hpp"

void Transform::Translate(const Vector3 dir) {
    const Vector3 relativeDir = right * dir.x + up * dir.y + forward * dir.z;
    position += relativeDir;
}

void Transform::RotateYaw(const float angle) {
    const glm::mat4 rotMat = glm::rotate(glm::mat4(1), glm::radians(angle), glm::vec3(Vector3::Up().x, Vector3::Up().y, Vector3::Up().z));
    const glm::vec4 rotatedVector = rotMat * glm::vec4(forward.x, forward.y, forward.z, 0);
    forward = Vector3(rotatedVector.x, rotatedVector.y, rotatedVector.z).Normalize();
    right = Vector3::CrossProduct(Vector3::Up(), forward).Normalize();
    up = Vector3::CrossProduct(forward, right);
    rotation.y += angle;
}

void Transform::RotatePitch(const float angle) {
    const glm::mat4 rotMat = glm::rotate(glm::mat4(1), glm::radians(angle), glm::vec3(right.x, right.y, right.z));
    const glm::vec4 rotatedVector = rotMat * glm::vec4(forward.x, forward.y, forward.z, 0);
    forward = Vector3(rotatedVector.x, rotatedVector.y, rotatedVector.z).Normalize();
    right = Vector3::CrossProduct(Vector3::Up(), forward);
    up = Vector3::CrossProduct(forward, right).Normalize();
    rotation.x += angle;
}


