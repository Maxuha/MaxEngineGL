//
// Created by zykov on 3/17/2026.
//

#include "Window.h"
#include <iostream>

#include "glad/glad.h"

void Window::MakeWindow() {
    if (!glfwInit()) {
        std::cout << "Failed to init GLFW\n";
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);


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

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);

    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::SwapBuffers() const {
    glfwSwapBuffers(window);
}

void Window::DestroyWindow() const {
    if (IsClosed()) return;

    glfwDestroyWindow(window);
    glfwTerminate();
}

bool Window::GetInputKey(const int key) const {
    return glfwGetKey(window, key) == GLFW_PRESS;
}

void Window::GetCursorPos(double *x, double *y) const {
    glfwGetCursorPos(window, x, y);
}

int Window::IsClosed() const {
    return glfwWindowShouldClose(window);
}
