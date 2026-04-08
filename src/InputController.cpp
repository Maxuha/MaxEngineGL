//
// Created by zykov on 4/6/2026.
//

#include "InputController.h"

InputController* InputController::Instance = nullptr;

InputController& InputController::GetInstance() {
    if (Instance == nullptr) {
        Instance = new InputController();
    }
    return *Instance;
}

void InputController::Init(GLFWwindow* window) {
    this->window = window;
}

bool InputController::GetKeyDown(const int key) const {
    return glfwGetKey(window, key) == GLFW_PRESS;
}

void InputController::GetCursorPos(double *x, double *y) const {
    glfwGetCursorPos(window, x, y);
}
