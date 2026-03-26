//
// Created by zykov on 3/15/2026.
//

#include "Cube.h"

#include <ranges>

#include "../../components/MeshRenderer.h"
#include "../../components/Transform.h"
#include "../../graphics/MeshPrimitives.h"

Cube* Cube::BuildCube() {
    const Mesh mesh = MeshPrimitives::CreateBox();
    const auto obj = new Cube();
    auto* renderer = obj->AddComponent<MeshRenderer>();
    renderer->mesh = mesh;
    renderer->gameObject = obj;
    auto* transform = obj->AddComponent<Transform>();
    transform->position = Vector3(0, 0, 0);
    return obj;
}

void Cube::Start() {
    GameObject::Start();

    transform = GetComponent<Transform>();
}

void Cube::Update(float delta_time) {
    GameObject::Update(delta_time);
    GetComponent<Transform>()->Translate(transform->Forward() * delta_time * 1);
}
