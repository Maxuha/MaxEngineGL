//
// Created by zykov on 3/17/2026.
//

#ifndef MAXENGINE_SCENE_H
#define MAXENGINE_SCENE_H
#include <string>
#include "gameObject/Camera.h"
#include "gameObject/light/DirectionalLight.h"
#include "gameObject/light/Light.h"
#include "graphics/RenderContext.h"

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

    std::vector<Light*> lights = {};

    std::vector<GameObject*> gameObjects = {};

    void Init() const;

    void Update(double delta_time);

    void Add(Light *light);

    void Add(GameObject *gameObject);

private:
    Camera *camera = nullptr;
    RenderContext render_context;

};


#endif //MAXENGINE_SCENE_H