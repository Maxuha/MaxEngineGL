//
// Created by zykov on 3/15/2026.
//

#include "Transform.h"

#include "glm/fwd.hpp"
#include "glm/ext/matrix_transform.hpp"

void Transform::Translate(const Vector3 dir) {
    auto rotationMatrix = glm::mat3(model);
    glm::vec3 localDir = rotationMatrix * glm::vec3(dir.x, dir.y, dir.z);

    position += Vector3(localDir.x, localDir.y, localDir.z) ;

    UpdateMatrix();
}

void Transform::RotateYaw(float angle) {
    rotation.y += angle;
    UpdateMatrix();
}

void Transform::RotatePitch(float angle) {
    rotation.x += angle;

    if (rotation.x > 89.0f) rotation.x = 89.0f;
    if (rotation.x < -89.0f) rotation.x = -89.0f;

    UpdateMatrix();
}

void Transform::UpdateMatrix() {
    auto mat = glm::mat4(1.0f);

    mat = glm::translate(mat, glm::vec3(position.x, position.y, position.z));

    mat = glm::rotate(mat, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    mat = glm::rotate(mat, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    //mat = glm::rotate(mat, glm::radians(rotation.z), glm::vec3(0, 0, 1)); // Roll

   // mat = glm::scale(mat, glm::vec3(scale.x, scale.y, scale.z));

    model = mat;

    right   = Vector3(model[0][0], model[0][1], model[0][2]).Normalize();
    up      = Vector3(model[1][0], model[1][1], model[1][2]).Normalize();
    forward = Vector3(model[2][0], model[2][1], model[2][2]).Normalize();
}



