//
// Created by zykov on 3/17/2026.
//

#include "Scene.h"

#include <iostream>

#include "components/MeshRenderer.h"
#include "gameObject/light/DirectionalLight.h"
#include "gameObject/primitives/Cube.h"
#include "IO/ObjImporter.h"
#include "IO/TextureImporter.h"
#include "math/AABB.h"

void Scene::Init() {
    light = new DirectionalLight();
    light->direction = Vector3(1.0f, -1.0f, -1.0f);
    light->intensity = 1.5f;
    light->color = Color(1.0f, 1.0f, 1.0f);

    camera = new Camera();
    camera->fov = 45;
    camera->near = 0.01;
    camera->far = 100;
    camera->aspectRatio = aspectRatio;

    auto *litShader = new Shader(
        (std::string(SHADERS_ROOT) + "/assets/shaders/basic_lit.vert").c_str(),
        (std::string(SHADERS_ROOT) + "/assets/shaders/basic_lit.frag").c_str()
    );

    TextureImporter textureImporter;
    Texture texture = textureImporter.Import(R"(C:/Users/zykov/CLionProjects/MaxEngine/assets/models/glove/texture/diffuse.jpg)");

    auto *defaultMaterial = new DefaultMaterial(litShader);

    defaultMaterial->color = Color(1.0f, 1.0f, 1.0f);
    defaultMaterial->texture = &texture;

    // Building a glove
    ObjImporter importer(R"(assets/glove.obj)");
    Mesh *mesh = importer.Import();

    auto glove = new GameObject();
    glove->name = "glove";

    auto *gloveMeshRenderer = glove->AddComponent<MeshRenderer>();
    gloveMeshRenderer->mesh = mesh;
    gloveMeshRenderer->gameObject = glove;
    gloveMeshRenderer->material = defaultMaterial;

    auto *gloveTransform = glove->GetComponent<Transform>();
    gloveTransform->pivot = mesh->center;

    const auto cube1 = Cube::BuildCube();
    cube1->GetComponent<MeshRenderer>()->material = defaultMaterial;
    const auto cube2 = Cube::BuildCube();
    cube2->GetComponent<MeshRenderer>()->material = defaultMaterial;

    gameObjects = { cube1, cube2, glove };

    for (GameObject *obj: gameObjects) {
        obj->Start();
    }
}

void Scene::Update(const float delta_time) {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gameObjects[0]->GetComponent<Transform>()->Translate(gameObjects[0]->GetComponent<Transform>()->Forward() * 4 * delta_time);
    gameObjects[1]->GetComponent<Transform>()->RotateYaw(-60.0f * delta_time);
    gameObjects[2]->GetComponent<Transform>()->RotateYaw(30.0f * delta_time);

    render_context.viewMatrix = camera->ViewMatrix();
    render_context.projectionMatrix = camera->ProjectionMatrix();
    render_context.lightDirection = light->direction;
    render_context.lightColor = light->color;
    render_context.lightIntensity = light->intensity;

    for (GameObject *obj: gameObjects) {
        obj->Update(delta_time);
        obj->GetComponent<MeshRenderer>()->Draw(render_context);
    }
}
