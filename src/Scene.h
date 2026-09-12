//
// Created by zykov on 3/17/2026.
//

#ifndef MAXENGINE_SCENE_H
#define MAXENGINE_SCENE_H
#include <string>

#include "components/MeshRenderer.h"
#include "components/light/Light.h"
#include "gameObject/Camera.h"
#include "renderer/DrawCall.h"

class Scene {
public:
    explicit Scene(Camera *camera) {
        this->camera = camera;
        gameObjects = {};
    }

    ~Scene() {
        delete camera;
        delete lights.data();
        //GOs are needed for delete too
    }

    std::string name;

    void Init() const;

    void Update(double delta_time);

    void Add(GameObject *gameObject);

    std::vector<MeshRenderer *> GetRenderableMeshes();

    std::vector<Rendering::DrawCall> GetDrawCalls();

    std::vector<Light *> GetLights();

    template<typename T>
        requires std::derived_from<T, Component>
    std::vector<T *> GetComponents() {
        std::vector<T *> allComponents;

        allComponents.reserve(gameObjects.size());

        for (GameObject *gameObject: gameObjects) {
            if (!gameObject) continue;

            std::vector<T *> chain = gameObject->GetComponentsInChildren<T>();

            allComponents.insert(allComponents.end(), chain.begin(), chain.end());
        }

        return allComponents;
    }

private:
    Camera *camera;

    std::vector<Light *> lights;

    std::vector<GameObject *> gameObjects;

    Shader *depthShader = nullptr;
    Shader *debugDepthQuad;

    unsigned int depthMapFBO;
    unsigned int depthMap;

    unsigned int planeVAO;
};


#endif //MAXENGINE_SCENE_H
