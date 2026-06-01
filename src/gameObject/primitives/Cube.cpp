//
// Created by zykov on 3/15/2026.
//

#include "Cube.h"
#include "../../components/MeshRenderer.h"
#include "../../components/Transform.h"
#include "../../graphics/MeshPrimitives.h"

Cube* Cube::BuildCube() {
    const auto mesh = std::shared_ptr<Mesh>(MeshPrimitives::CreateBox());
    auto* obj = new Cube();
    auto* renderer = obj->AddComponent<MeshRenderer>();
    renderer->mesh = mesh;
    return obj;
}

void Cube::Start() {
    GameObject::Start();
    transform = GetComponent<Transform>();
}

void Cube::Update(const float delta_time) {
    GameObject::Update(delta_time);
}
