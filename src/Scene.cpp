//
// Created by zykov on 3/17/2026.
//

#include "Scene.h"
#include "components/MeshRenderer.h"
#include "gameObject/light/DirectionalLight.h"
#include "gameObject/primitives/Cube.h"
#include "IO/ObjImporter.h"
#include "math/AABB.h"

void Scene::Init() const {
    for (GameObject *obj: gameObjects) {
        obj->Start();
    }
}

void Scene::Update(const double delta_time) {
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      gameObjects[0]->GetComponent<Transform>()->Translate(gameObjects[0]->GetComponent<Transform>()->Forward() * 2.0f * delta_time);
    //  gameObjects[3]->GetComponent<Transform>()->RotateYaw(30.0f * delta_time);
     // gameObjects[1]->GetComponent<Transform>()->RotateYaw(-50.0f * delta_time);
     // gameObjects[2]->GetComponent<Transform>()->RotateYaw(30.0f * delta_time);

    camera->Update(delta_time);

    for (GameObject *obj: gameObjects) {
        obj->Update(delta_time);
        const auto* meshRenderer = obj->GetComponent<MeshRenderer>();

        std::vector<MeshRenderer*> renderers;

        GetMeshRenderer(obj->GetTransform(), renderers);

       // renderers.push_back(->GetGameObject()->GetComponent<MeshRenderer>());

        for (const auto& renderer: renderers) {
            if (renderer == nullptr) continue;
            renderer->Draw(lights, *camera);
        }

        // if (meshRenderer != nullptr) {
        //     meshRenderer->Draw(lights, *camera);
        // }

        // for (const Transform* child: obj->GetTransform()->children) {
        //     const auto* childRenderer = child->GetGameObject()->GetComponent<MeshRenderer>();
        //     childRenderer->Draw(lights, *camera);
        // }
    }
}

void Scene::Add(Light *light) {
    lights.push_back(light);
}

void Scene::Add(GameObject *gameObject) {
    gameObjects.push_back(gameObject);
}

Transform* Scene::GetMeshRenderer(Transform *transform, std::vector<MeshRenderer*>& meshRenderers) {
    meshRenderers.push_back(transform->GetGameObject()->GetComponent<MeshRenderer>());

    for (const auto& pair: transform->children) {
        meshRenderers.push_back(pair->GetGameObject()->GetComponent<MeshRenderer>());
        GetMeshRenderer(pair, meshRenderers);
    }
    return transform;
}
