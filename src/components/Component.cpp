//
// Created by zykov on 3/15/2026.
//

#include "Component.h"

Component::Component() {
}

Component::Component(GameObject* gameObject) {
     this->gameObject = gameObject;
}

void Component::Attach(GameObject &_gameObject) {
    this->gameObject = &_gameObject;
}

GameObject * Component::GetGameObject() const {
    return gameObject;
}

void Component::Start() {
}

void Component::Update(float delta_time) {
}
