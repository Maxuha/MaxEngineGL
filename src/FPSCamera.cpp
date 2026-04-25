//
// Created by zykov on 4/6/2026.
//

#include "FPSCamera.h"

#include "InputController.h"

FPSCamera::FPSCamera(float fov, float near, float far, float aspectRatio) : Camera(fov, near, far, aspectRatio){
}

void FPSCamera::Start() {
    Camera::Start();
}

void FPSCamera::Update(float delta_time) {
    Camera::Update(delta_time);

    auto* cameraTransform = GetComponent<Transform>();

    auto direction = Vector3::Zero();

    if (InputController::GetInstance().GetKeyDown(GLFW_KEY_W)) {
        direction += cameraTransform->Forward();
    }
    if (InputController::GetInstance().GetKeyDown(GLFW_KEY_S)) {
        direction -= cameraTransform->Forward();
    }
    if (InputController::GetInstance().GetKeyDown(GLFW_KEY_A)) {
        direction -= cameraTransform->Right();
    }
    if (InputController::GetInstance().GetKeyDown(GLFW_KEY_D)) {
        direction += cameraTransform->Right();
    }

    if (InputController::GetInstance().GetKeyDown(GLFW_KEY_LEFT_SHIFT)) {
        cameraSpeed = maxCameraSpeed;
    } else {
        cameraSpeed = minCameraSpeed;
    }

    if (InputController::GetInstance().GetKeyDown(GLFW_KEY_W) ||
        InputController::GetInstance().GetKeyDown(GLFW_KEY_S) ||
        InputController::GetInstance().GetKeyDown(GLFW_KEY_A) ||
        InputController::GetInstance().GetKeyDown(GLFW_KEY_D)) {
            cameraTransform->Translate(direction * cameraSpeed * delta_time);
        }

    double x, y;

    InputController::GetInstance().GetCursorPos(&x, &y);

    directionRot = Vector3(x - directionRot.x, y - directionRot.y, directionRot.z);

    const float Yaw = directionRot.x * delta_time * 30;
    float Pitch = directionRot.y * delta_time * 30;

    if (Yaw != 0 || Pitch != 0) {
        cameraTransform->RotateYaw(Yaw);
        cameraTransform->RotatePitch(Pitch);
    }
    directionRot = Vector3(x, y, directionRot.z);
}

FPSCamera::~FPSCamera() {
}
