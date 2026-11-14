//
// Created by zykov on 10/28/2026.
//

#include "Sphere.h"

#include "../../renderer/domain/MeshPrimitives.h"
#include "../src/components/MeshRenderer.h"

Sphere * Sphere::BuildSphere() {
    const auto mesh = std::shared_ptr<Mesh>(MeshPrimitives::CreateSphere(1));
    auto* obj = new Sphere();
    auto* renderer = obj->AddComponent<MeshRenderer>();
    renderer->mesh = mesh;
    return obj;
}

void Sphere::Start() {
    GameObject::Start();
}

void Sphere::Update(float delta_time) {
    GameObject::Update(delta_time);
}
