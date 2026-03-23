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
    camera->near = 0.01;
    camera->far = 1000;
    camera->aspectRatio = aspectRatio;

    gameObject = { Cube::BuildCube() };

    for (GameObject *obj : gameObject) {
        obj->Start();
    }
}

void Scene::Render() {
    render_context.viewMatrix = camera->ViewMatrix();
    render_context.projectionMatrix = camera->ProjectionMatrix();
    render_context.lightDirection = light->direction;
    render_context.lightColor = light->color;
    render_context.lightIntensity = light->intensity;

    for (GameObject *obj : gameObject) {
        obj->GetComponent<MeshRenderer>()->Render(render_context);
        obj->Update();
    }
}

void Scene::Cleanup() {
}
