//
// Created by zykov on 3/17/2026.
//

#ifndef MAXENGINE_SCENE_H
#define MAXENGINE_SCENE_H
#include <string>

#include "components/MeshRenderer.h"
#include "gameObject/Camera.h"
#include "gameObject/light/Light.h"

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

    void Add(Light *light);

    void Add(GameObject *gameObject);

private:
    Camera *camera;

    std::vector<Light *> lights;

    std::vector<GameObject *> gameObjects;

    Transform* GetMeshRenderer(Transform* transform, std::vector<MeshRenderer*>& meshRenderers);
};


#endif //MAXENGINE_SCENE_H
