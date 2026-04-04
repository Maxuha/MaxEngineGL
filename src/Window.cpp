//
// Created by zykov on 3/17/2026.
//

#include "Window.h"
#include <iostream>

#include "glad/glad.h"

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

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);
}

void Window::SwapBuffers() const {
    glfwSwapBuffers(window);
}

void Window::Close() const {
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
