//
// Created by zykov on 3/17/2026.
//

#include "Scene.h"



#include "renderer/domain/LightItemRenderData.h"

using namespace Rendering;

class AssetManager;

void Scene::Init() const {
    for (GameObject *obj: gameObjects) {
        obj->Start();
    }
}

void Scene::Update(const double delta_time) {
}

void Scene::Add(GameObject *gameObject) {
    gameObjects.push_back(gameObject);
}

std::vector<MeshRenderer *> Scene::GetRenderableMeshes() {
    std::vector<MeshRenderer *> renderable_meshes;
    renderable_meshes.reserve(gameObjects.size());
    for (const auto game_object : gameObjects) {
        if (auto mesh_renderer = game_object->GetComponent<MeshRenderer>()) {
            renderable_meshes.push_back(mesh_renderer);
        }
    }
    return renderable_meshes;
}

std::vector<DrawCall> Scene::GetDrawCalls() {
    const std::vector<MeshRenderer *> renderers = GetComponents<MeshRenderer>();
    std::vector<DrawCall> drawCalls;
    drawCalls.reserve(renderers.size());
    for (const auto renderer : renderers) {
        if (renderer != nullptr && renderer->mesh != nullptr && renderer->material != nullptr) {
            drawCalls.push_back(DrawCall { ._mesh = renderer->mesh.get(), .material = renderer->material , .transform = renderer->GetGameObject()->GetTransform()});
        }
    }
    return drawCalls;
}

std::vector<Light *> Scene::GetLights() {
    const std::vector<Light *> lights = GetComponents<Light>();
    Light light;

    return lights;
}
