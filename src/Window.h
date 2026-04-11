//
// Created by zykov on 3/17/2026.
//

#ifndef MAXENGINE_WINDOW_H
#define MAXENGINE_WINDOW_H

#include "Scene.h"
#include "GLFW/glfw3.h"

class Window {
public:
    Window(const int width, const int height) {
        this->width = width;
        this->height = height;
        aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    }

    void AttachScene(Scene* scene);

    void Open();

    void Update();

    void Close() const;

    int IsClosed() const;

private:
    GLFWwindow *window{};
    Scene* scene{};

    int width, height;
    float aspectRatio;

    double last_time = glfwGetTime();
    double delta_time = 0;
};


#endif //MAXENGINE_WINDOW_H