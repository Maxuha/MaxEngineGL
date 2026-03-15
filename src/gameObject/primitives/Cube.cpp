//
// Created by zykov on 3/15/2026.
//

#include "Cube.h"

#include "../../components/MeshRenderer.h"
#include "../../components/Transform.h"
#include "../../graphics/MeshPrimitives.h"

Cube Cube::BuildCube(Camera* camera) {
    Mesh mesh = MeshPrimitives::CreateBox();
    Cube obj;
    auto* renderer = obj.AddComponent<MeshRenderer>();
    renderer->position = Vector3(0.0f, 0.0f, 0.0f);
    renderer->mesh = mesh;
    renderer->camera = camera;
    // renderer->gameObject = obj;
    auto* transform = obj.AddComponent<Transform>();
    transform->position = Vector3(2, 0, -3);
    transform->forward = Vector3::Forward();
    transform->up = Vector3::Up();
    return obj;
}
