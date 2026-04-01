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
    Scene(const int width, const int height) {
        aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    }

    ~Scene() {
        delete camera;
        delete light;
        //GOs are needed for delete too
    }

    float aspectRatio;

    std::string name;

    Camera *camera = nullptr;
    DirectionalLight *light = nullptr;

    std::vector<GameObject*> gameObjects = {};

    void Init();

    void Render(float delta_time);

private:
    RenderContext render_context;

};


#endif //MAXENGINE_SCENE_H