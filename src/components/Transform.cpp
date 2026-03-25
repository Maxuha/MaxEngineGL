//
// Created by zykov on 3/15/2026.
//

#include "Transform.h"

#include <iostream>

#include "glm/fwd.hpp"
#include "glm/ext/matrix_transform.hpp"

Vector3 Transform::position() {
    return Vector3(model[3][0], model[3][1], model[3][2]);
}

Vector3 Transform::rotation() {
    return Vector3::Zero();
}

Vector3 Transform::scale() {
    return Vector3::Zero();
}

Vector3 Transform::right() {
    return Vector3(model[0][0], model[0][1], model[0][2]).Normalize();
}

Vector3 Transform::up() {
    return Vector3(model[1][0], model[1][1], model[1][2]).Normalize();
}

Vector3 Transform::forward() {
    return Vector3(model[2][0], model[2][1], model[2][2]).Normalize();
}

void Transform::Translate(const Vector3 dir) {
    model = glm::translate(model, glm::vec3(dir.x, dir.y, dir.z));
}

void Transform::RotateYaw(const float angle) {
    model = glm::rotate(model, glm::radians(angle), glm::vec3(Vector3::Up().x, Vector3::Up().y, Vector3::Up().z));
}

void Transform::RotatePitch(const float angle) {
    std::cout << "right: " << right().x << right().y << right().z << std::endl;
    model = glm::rotate(model, glm::radians(angle), glm::vec3(Vector3::Right().x, Vector3::Right().y, Vector3::Right().z));
}

void Transform::UpdateMatrix() {
    //auto mat = glm::mat4(1);

    //std::cout << "rotation: " << rotation.x << " " << rotation.y << " " << rotation.z << std::endl;
    //
    //  auto rotMat  = glm::rotate(glm::mat4(1), glm::radians(rotation.y), glm::vec3(Vector3::Up().x, Vector3::Up().y, Vector3::Up().z));
    // //mat = glm::rotate(mat, glm::radians(rotation.x), glm::vec3(right.x, right.y, right.z));
    //
    // model = rotMat;
    //
    // right   = Vector3(model[0][0], model[0][1], model[0][2]).Normalize();
    // up      = Vector3(model[1][0], model[1][1], model[1][2]).Normalize();
    // forward = Vector3(model[2][0], model[2][1], model[2][2]).Normalize();

}


