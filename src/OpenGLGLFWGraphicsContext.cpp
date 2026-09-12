//
// Created by zykov on 6/13/2026.
//

#include "OpenGLGLFWGraphicsContext.h"

#include <iostream>

#include "IWindow.h"
#include "GLFW/glfw3.h"

OpenGLGLFWGraphicsContext::~OpenGLGLFWGraphicsContext() {
}

void OpenGLGLFWGraphicsContext::Init() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // error handler
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

}

void OpenGLGLFWGraphicsContext::SetContext(IWindow* window) {
    glfwMakeContextCurrent(static_cast<GLFWwindow *>(window->GetHandle()));

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(window->GetProcAddress()))) {
        std::cout << "Failed to initialize GLAD\n";
        window->Close();
    }

    const GLubyte *version = glGetString(GL_VERSION);
    std::cout << "OpenGL Version: " << version << std::endl;

    // error log
    glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity,
                              GLsizei length, const GLchar *message, const void *userParam) {
        if (severity == GL_DEBUG_SEVERITY_HIGH) {
            printf("OpenGL: %s\n", message);
#ifdef _MSC_VER
            __debugbreak();
#endif
        }
    }, nullptr);

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
}
