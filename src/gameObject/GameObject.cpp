//
// Created by zykov on 3/15/2026.
//

#include "../gameObject/GameObject.h"
#include "../components/Transform.h"

GameObject::GameObject() {
    AddComponent<Transform>();
}

void GameObject::Start() {
}

void GameObject::Update(float delta_time) {
    //GetComponent<MeshRenderer>()->Render();
}
