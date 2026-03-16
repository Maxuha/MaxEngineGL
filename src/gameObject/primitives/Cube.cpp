//
// Created by zykov on 3/15/2026.
//

#include "Cube.h"

#include "../../components/MeshRenderer.h"
#include "../../components/Transform.h"
#include "../../graphics/MeshPrimitives.h"

Cube* Cube::BuildCube(Camera* camera) {
    Mesh mesh = MeshPrimitives::CreateBox();
    Cube* obj = new Cube();
    auto* renderer = obj->AddComponent<MeshRenderer>();
    renderer->mesh = mesh;
    renderer->camera = camera;
    renderer->gameObject = obj;
    auto* transform = obj->AddComponent<Transform>();
    transform->position = Vector3(0, 0, 0);
    transform->forward = Vector3::Forward();
    transform->up = Vector3::Up();
    return obj;
}
