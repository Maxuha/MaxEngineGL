//
// Created by zykov on 3/17/2026.
//

#ifndef MAXENGINE_SCENE_H
#define MAXENGINE_SCENE_H
#include <string>

#include "components/MeshRenderer.h"
#include "components/light/Light.h"
#include "gameObject/Camera.h"

class Window;

class Scene {
public:
    explicit Scene(Camera *camera) {
        this->camera = camera;
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

    template<typename T>
    requires std::derived_from<T, Component>
    std::vector<T*> GetComponents() {
        std::vector<T*> components;
        for (GameObject* gameObject : gameObjects) {
            auto component = gameObject->GetComponent<T>();
            if (component != nullptr) {
                components.push_back(component);
            }
            auto componentsInChildren = gameObject->GetComponentsInChildren<T>();
            components.insert(components.end(), componentsInChildren.begin(), componentsInChildren.end());
        }
        return components;
    }

private:
    Camera *camera;

    std::vector<Light *> lights;

    std::vector<GameObject *> gameObjects;

    Shader* depthShader = nullptr;
    Shader* debugDepthQuad;

    unsigned int depthMapFBO;
    unsigned int depthMap;

    unsigned int planeVAO;
};


#endif //MAXENGINE_SCENE_H
