//
// Created by zykov on 3/17/2026.
//

#include "Scene.h"

#include "components/MeshRenderer.h"
#include "gameObject/light/DirectionalLight.h"
#include "gameObject/primitives/Cube.h"

void Scene::Init() {

    light = new DirectionalLight();

    camera = new Camera();
    camera->fov = 45;
    camera->near = 0.1;
    camera->far = 100;
    camera->aspectRatio = aspectRatio;

    gameObject = { Cube::BuildCube() };

    for (GameObject *obj : gameObject) {
        obj->Start();
    }
}

void Scene::Render(float delta_time) {
    render_context.viewMatrix = camera->ViewMatrix();
    render_context.projectionMatrix = camera->ProjectionMatrix();
    render_context.lightDirection = light->direction;
    render_context.lightColor = light->color;
    render_context.lightIntensity = light->intensity;

    for (GameObject *obj : gameObject) {
        obj->Update(delta_time);
        obj->GetComponent<MeshRenderer>()->Render(render_context);
    }
}

void Scene::Cleanup() {
}
