//
// Created by zykov on 3/15/2026.
//

#include "Cube.h"

#include <iostream>

#include "../../components/MeshRenderer.h"
#include "../../components/Transform.h"
#include "../../graphics/MeshPrimitives.h"
#include "../../IO/ObjImporter.h"

Cube* Cube::BuildCube() {
    // const Mesh mesh = MeshPrimitives::CreateBox();
    const Mesh mesh = ObjImporter::Import(R"(C:\Users\zykov\CLionProjects\MaxEngine\assets\VideoShip.obj)");
    // std::cout << "Mesh" << std::endl;
    //
    // for (int i = 0; i < mesh.tries.size(); i++) {
    //     std::cout << mesh.tries[i].vertices->position.x << " " << mesh.tries[i].vertices->position.y << " " << mesh.tries[i].vertices->position.z << std::endl;
    // }

    std::cout << "mesh: " << mesh.tries.size() << std::endl;
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
    GetComponent<Transform>()->Translate(transform->Forward() * delta_time * 2);
    //GetComponent<Transform>()->RotateYaw(delta_time * 30);
}
