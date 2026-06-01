//
// Created by zykov on 3/17/2026.
//

#ifndef MAXENGINE_WINDOW_H
#define MAXENGINE_WINDOW_H

#include "IWindow.h"
#include "Scene.h"
#include "GLFW/glfw3.h"
#include "renderer/domain/IWindowContext.h"

class Window : public IWindow, public Rendering::IWindowContext {
public:
    Window(const int width, const int height) {
        this->width = width;
        this->height = height;
        aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    }

    void Open() override;

    void Update() override;

    void Close() const override;

    bool IsOpen() const override;

    void AddView(Scene& scene) override;

    void * GetNativeHandle() override;



private:
    GLFWwindow *window{};
    Scene* scene{};

    int width, height;
    float aspectRatio;

    double last_time = glfwGetTime();
    double delta_time = 0;

    void DoUpdate();
};


#endif //MAXENGINE_WINDOW_H