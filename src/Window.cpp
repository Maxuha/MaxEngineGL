//
// Created by zykov on 3/17/2026.
//

#include "Window.h"
#include <iostream>
#include "InputController.h"
#include "di/DIContainer.h"
#include "glad/glad.h"

class Transform;

void Window::Open() {
    if (!glfwInit()) {
        std::cout << "Failed to init GLFW\n";
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // error handler
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

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
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glViewport(0, 0, width, height);

    glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity,
                              GLsizei length, const GLchar *message, const void *userParam) {
        if (severity == GL_DEBUG_SEVERITY_HIGH) {
            printf("OpenGL ??????????? ??????: %s\n", message);
#ifdef _MSC_VER
            __debugbreak();
#endif
        }
    }, nullptr);
}

void Window::Update() {
    while (IsOpen()) {
        DoUpdate();
    }
}

void Window::Close() const {
    if (!IsOpen()) return;

    glfwDestroyWindow(window);
    glfwTerminate();
}

bool Window::IsOpen() const {
    return glfwWindowShouldClose(window) == 0;
}

void Window::AddView(Scene &scene) {
    this->scene = &scene;
    scene.Init();
}

void * Window::GetNativeHandle() {
    return window;
}

void Window::DoUpdate() {
    // calculate delta time
    delta_time = glfwGetTime() - last_time;
    last_time = glfwGetTime();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        Close();
    }

    scene->Update(delta_time);
    DIContainer::GetInstance().Get<PhysicsEngine>()->Update();

    glfwPollEvents();
}
