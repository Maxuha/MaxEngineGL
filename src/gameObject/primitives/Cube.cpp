//
// Created by zykov on 3/15/2026.
//

#include "Cube.h"

#include <iostream>
#include <ostream>

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
   // transform->position = Vector3(0, 0, 0);
    // transform->forward = Vector3::Forward();
    // transform->up = Vector3::Up();
    // transform->right = Vector3::Right();
    return obj;
}

void Cube::Start() {
    GameObject::Start();

        //GetComponent<Transform>()->Translate(Vector3::Up());
}

void Cube::Update(float delta_time) {
    GameObject::Update(delta_time);
    auto right = GetComponent<Transform>()->position;
    auto up = GetComponent<Transform>()->rotation;
    auto forward = GetComponent<Transform>()->scale;

    // std::cout << right.x << " " << right.y << " " << right.z << std::endl;
    // std::cout << forward.x << " " << forward.y << " " << forward.z << std::endl;
    // std::cout << up.x << " " << up.y << " " << up.z << std::endl;
   // GetComponent<Transform>()->Translate(Vector3::Forward() * delta_time * 1);
    //GetComponent<Transform>()->RotateYaw(60.0f * delta_time);
    //GetComponent<Transform>()->UpdateMatrix();
}
