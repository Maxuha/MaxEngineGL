//
// Created by zykov on 6/15/2026.
//

#include "OGLGraphicsContext.h"

#include "glad/glad.h"

namespace Rendering {
    OGLGraphicsContext::OGLGraphicsContext(IWindow *window) {
        this->window = window;

        window->MakeOGLContext();

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
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
    }

    void OGLGraphicsContext::Init() {

    }
} // Rendering