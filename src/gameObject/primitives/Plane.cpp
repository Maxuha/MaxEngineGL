//
// Created by zykov on 10/15/2026.
//

#include "Plane.h"

#include "../../renderer/domain/MeshPrimitives.h"
#include "../../components/MeshRenderer.h"
#include "../../components/Transform.h"

Plane * Plane::BuildPlane() {
    const auto mesh = std::shared_ptr<Mesh>(MeshPrimitives::CreatePlane());
    const auto plane = new Plane();
    plane->AddComponent<MeshRenderer>();
    plane->GetComponent<MeshRenderer>()->mesh = mesh;
    return plane;
}

void Plane::Start() {
    GameObject::Start();
    transform = GetComponent<Transform>();
}

void Plane::Update(const float delta_time) {
    GameObject::Update(delta_time);
}
