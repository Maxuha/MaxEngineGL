//
// Created by zykov on 4/6/2026.
//

#include "InputController.h"

#include "IKeyListener.h"
#include "math/Vector2.h"

InputController::InputController(IInputContext* context) : context(context) {
    context->AddKeyListener([this](const int key, const int scancode, const int action, const int mods) {
        KeyPressed(key, scancode, action, mods);
    });
}

void InputController::AddKeyListener(IKeyListener *listener) {
    keyListeners.push_back(listener);
}

void InputController::RemoveKeyListener(IKeyListener *listener) {
    std::erase(keyListeners, listener);
}

bool InputController::GetKeyDown(const int keycode) const {
    return context->GetKeyDown(keycode);
}

Vector2 InputController::GetCursorPos() const {
    return context->GetMousePosition();
}

void InputController::Update() {
    glfwPollEvents();
}

void InputController::KeyPressed(const int key, const int scancode, const int action, const int mods) const {
    for (const auto listener : keyListeners) {
        listener->OnKeyPressed(key, scancode, action, mods);
    }
}
