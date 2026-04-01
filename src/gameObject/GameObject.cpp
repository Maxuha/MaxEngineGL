//
// Created by zykov on 3/15/2026.
//

#include "../gameObject/GameObject.h"

#include "../components/MeshRenderer.h"

void GameObject::Start() {
    GetComponent<MeshRenderer>()->Init();
}

void GameObject::Update(float delta_time) {
    //GetComponent<MeshRenderer>()->Render();
}

void GameObject::InternalUpdate() {
    //Update();
    //GetComponent<MeshRenderer>()->Render();
}
