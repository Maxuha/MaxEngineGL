//
// Created by zykov on 6/26/2026.
//

#include "OGLInstance.h"
#include "../src/IWindow.h"
#include "GLFW/glfw3.h"

namespace Rendering {
    OGLInstance::OGLInstance(IWindow &window) {
        window.MakeOGLContext();

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
            std::cout << "Failed to initialize GLAD\n";
            window.Close();
        }

        const GLubyte *version = glGetString(GL_VERSION);
        std::cout << "OpenGL Version: " << version << std::endl;

        // error log
        glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, const GLenum severity,
                                  GLsizei length, const GLchar *message, const void *userParam) {
            if (severity == GL_DEBUG_SEVERITY_HIGH) {
                printf("OpenGL: %s\n", message);
#ifdef _MSC_VER
                __debugbreak();
#else
                __builtin_trap();
#endif
            }
        }, nullptr);

        glClipControl(GL_UPPER_LEFT, GL_ZERO_TO_ONE);
        glEnable(GL_FRAMEBUFFER_SRGB);

        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    }

    OGLInstance::~OGLInstance() {
    }
} // Rendering
