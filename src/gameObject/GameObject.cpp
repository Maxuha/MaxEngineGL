//
// Created by zykov on 3/15/2026.
//

#include "../gameObject/GameObject.h"
#include "../components/Transform.h"

GameObject::GameObject(const std::string &name) {
    transform = AddComponent<Transform>();
    this->name = name;
}

GameObject::~GameObject() {
}

void GameObject::Start() {
}

void GameObject::Update(const float delta_time) {
    // for (const auto& pair: components) {
    //     pair.second->Update(delta_time);
    // }
}
