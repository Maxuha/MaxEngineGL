//
// Created by zykov on 3/15/2026.
//

#include "Transform.h"

#include "glm/fwd.hpp"
#include "glm/ext/matrix_transform.hpp"

Vector3 Transform::Right() const {
    return Vector3::CrossProduct(up, forward);
}

void Transform::Translate(Vector3 dir) {
    Vector3 relativeDir = Right() * dir.x + up * dir.y + forward * dir.z;
    position += relativeDir;
}

void Transform::RotateYaw(float angle) {
    glm::mat4 rotMat = glm::rotate(glm::mat4(1), angle, glm::vec3(Vector3::Up().x, Vector3::Up().y, Vector3::Up().z));
    glm::vec4 rotatedVector = glm::vec4(forward.x, forward.y, forward.z, 0) * rotMat;
    forward = Vector3(rotatedVector.x, rotatedVector.y, rotatedVector.z);
}


