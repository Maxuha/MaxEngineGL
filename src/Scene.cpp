//
// Created by zykov on 3/17/2026.
//

#include "Scene.h"

#include <iostream>

#include "components/MeshRenderer.h"
#include "gameObject/light/DirectionalLight.h"
#include "gameObject/primitives/Cube.h"
#include "IO/ObjImporter.h"
#include "math/AABB.h"

void Scene::Init() {
    for (GameObject *obj: gameObjects) {
        obj->Start();
    }
}

void Scene::Update(const double delta_time) {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

     gameObjects[0]->GetComponent<Transform>()->RotateYaw(10.0f * delta_time);
     gameObjects[1]->GetComponent<Transform>()->RotateYaw(-50.0f * delta_time);
     gameObjects[2]->GetComponent<Transform>()->RotateYaw(30.0f * delta_time);

    render_context.viewMatrix = camera->ViewMatrix();
    render_context.projectionMatrix = camera->ProjectionMatrix();
    render_context.lightDirection = dynamic_cast<DirectionalLight*>(lights[0])->direction;
    render_context.lightColor = dynamic_cast<DirectionalLight*>(lights[0])->color;
    render_context.lightIntensity = dynamic_cast<DirectionalLight*>(lights[0])->intensity;

    for (GameObject *obj: gameObjects) {
        camera->Update(delta_time);
        obj->Update(delta_time);
        obj->GetComponent<MeshRenderer>()->Draw(render_context);
    }
}

void Scene::AddLight(Light *light) {
    lights.push_back(light);
}

void Scene::AddGameObject(GameObject *gameObject) {
    gameObjects.push_back(gameObject);
}
