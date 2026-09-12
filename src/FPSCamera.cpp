//
// Created by zykov on 4/6/2026.
//

#include "FPSCamera.h"
#include "InputController.h"
#include "di/DIContainer.h"

FPSCamera::FPSCamera(const float fov, const float zNear, const float zFar, const float aspectRatio) : Camera(fov, zNear, zFar, aspectRatio){
}

void FPSCamera::Start() {
    Camera::Start();
   // const auto controller = DIContainer::GetInstance().Get<InputController>();
    //controller->AddKeyListener(this);
}

void FPSCamera::Update(const float delta_time) {
    Camera::Update(delta_time);
    this->deltaTime = delta_time;
     auto* cameraTransform = GetComponent<Transform>();
    //
     auto direction = Vector3::Zero();
    //
     const auto controller = DIContainer::GetInstance().Get<InputController>();

    if (controller->GetKeyDown(GLFW_KEY_W)) {
        direction += cameraTransform->Forward();
    }
    if (controller->GetKeyDown(GLFW_KEY_S)) {
        direction -= cameraTransform->Forward();
    }
    if (controller->GetKeyDown(GLFW_KEY_A)) {
        direction -= cameraTransform->Right();
    }
    if (controller->GetKeyDown(GLFW_KEY_D)) {
        direction += cameraTransform->Right();
    }

    //
    if (controller->GetKeyDown(GLFW_KEY_LEFT_SHIFT)) {
        cameraSpeed = maxCameraSpeed;
    } else {
        cameraSpeed = minCameraSpeed;
    }
    //
    if (controller->GetKeyDown(GLFW_KEY_W) ||
        controller->GetKeyDown(GLFW_KEY_S) ||
        controller->GetKeyDown(GLFW_KEY_A) ||
        controller->GetKeyDown(GLFW_KEY_D)) {
            cameraTransform->Translate(direction * cameraSpeed * delta_time);
        }

    // double x, y;
    //
     Vector2 vector2 = controller->GetCursorPos();
    //
    directionRot = Vector3(vector2.x - directionRot.x,vector2.y - directionRot.y, directionRot.z);

    const float Yaw = -directionRot.x * delta_time * 30;
    const float Pitch = -directionRot.y * delta_time * 30;

    if (Yaw != 0 || Pitch != 0) {
        cameraTransform->RotateYaw(Yaw);
        cameraTransform->RotatePitch(Pitch);
    }

    directionRot = Vector3(vector2.x, vector2.y, directionRot.z);
}

void FPSCamera::OnKeyPressed(const int key, int scancode, const int action, int mods) {
    cameraSpeed = minCameraSpeed;
    if (action != GLFW_PRESS) return;

    auto* cameraTransform = GetComponent<Transform>();

    auto direction = Vector3::Zero();

    switch (key) {
        case GLFW_KEY_W: direction += cameraTransform->Forward(); break;
        case GLFW_KEY_S: direction -= cameraTransform->Forward(); break;
        case GLFW_KEY_A: direction -= cameraTransform->Right(); break;
        case GLFW_KEY_D: direction += cameraTransform->Right(); break;
        case GLFW_KEY_LEFT_SHIFT: cameraSpeed = maxCameraSpeed;
        default: cameraSpeed = minCameraSpeed;
    }

    cameraTransform->Translate(direction * cameraSpeed * deltaTime * 100);
}

FPSCamera::~FPSCamera() {
}
