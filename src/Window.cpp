//
// Created by zykov on 3/17/2026.
//

#include "Window.h"
#include <iostream>

#include "InputController.h"
#include "glad/glad.h"
#include "math/Vector3.h"

class Transform;

void Window::AttachScene(Scene *scene) {
    this->scene = scene;
    scene->Init();
}

void Window::Open() {
    if (!glfwInit()) {
        std::cout << "Failed to init GLFW\n";
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "Max Engine", nullptr, nullptr);

    if (!window) {
        std::cout << "Failed to create window\n";
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD\n";
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    InputController::GetInstance().Init(window);

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);
}

void Window::Update() {
    // calculate delta time
    delta_time = glfwGetTime() - last_time;
    last_time = glfwGetTime();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        Close();
    }

    scene->Update(delta_time);

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Window::Close() const {
    if (IsClosed()) return;

    glfwDestroyWindow(window);
    glfwTerminate();
}

int Window::IsClosed() const {
    return glfwWindowShouldClose(window);
}
