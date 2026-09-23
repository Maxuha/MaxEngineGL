//
// Created by zykov on 3/17/2026.
//

#include "GLFWWindowImpl.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "InputController.h"
#include <vulkan/vulkan.h>


GLFWWindowImpl::GLFWWindowImpl(const WindowDesc& desc, IGLFWGraphicsContext* graphicsContext) {
    this->width = desc.width;
    this->height = desc.height;
    aspectRatio = static_cast<float>(width) / static_cast<float>(height);

    if (!glfwInit()) {
        std::cout << "Failed to init GLFW\n";
    }

    graphicsContext->Init();

    window = glfwCreateWindow(desc.width, desc.height, "Max Engine", nullptr, nullptr);

    if (!window) {
        std::cout << "Failed to create window\n";
        glfwTerminate();
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    int api = glfwGetWindowAttrib(window, GLFW_CLIENT_API);

    if (api == GLFW_OPENGL_API) {
        std::cout << "Window is using OpenGL" << std::endl;
    } else if (api == GLFW_OPENGL_ES_API) {
        std::cout << "Window is using OpenGL ES" << std::endl;
    } else if (api == GLFW_NO_API) {
        std::cout << "Window is using Vulkan" << std::endl;
    }
}

GLFWWindowImpl::~GLFWWindowImpl() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void GLFWWindowImpl::Close() const {
    if (!IsOpen()) return;

    glfwDestroyWindow(window);
    glfwTerminate();
}

bool GLFWWindowImpl::IsOpen() const {
    return glfwWindowShouldClose(window) == 0;
}

double GLFWWindowImpl::GetDeltaTime() {
    delta_time = glfwGetTime() - last_time;
    last_time = glfwGetTime();
    return delta_time;
}

void * GLFWWindowImpl::GetNativeHandle() {
    return window;
}

void * GLFWWindowImpl::GetHandle() {
    return window;
}

void GLFWWindowImpl::SetApi(const GraphicsApi api) {
    switch (api) {
        case GraphicsApi::OpenGL: {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            // error handler
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
        }
            break;
            case GraphicsApi::Vulkan: {
                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
            }
            break;
        default: ;
    }
}

void GLFWWindowImpl::MakeOGLContext() {
    glfwMakeContextCurrent(window);
}

void * GLFWWindowImpl::GetProcAddress() {
    return reinterpret_cast<void*>(glfwGetProcAddress);
}

bool GLFWWindowImpl::CheckVulkanSupport() {
    return glfwVulkanSupported();
}

const char **GLFWWindowImpl::GetExtensions(uint32_t& count) {
    return glfwGetRequiredInstanceExtensions(&count);
}

bool GLFWWindowImpl::CreateSurface(VkInstance& instance, VkSurfaceKHR &surface) {
    return glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS;
}

HWND GLFWWindowImpl::GetHWND() {
    return glfwGetWin32Window(window);
}

Rect GLFWWindowImpl::GetCurrentSize() {
    glfwGetWindowSize(window, &width, &height);
    return Rect { .x = 0, .y = 0, .width = width, .height = height };
}

bool GLFWWindowImpl::isKeyPressed(const int keycode) {
    return glfwGetKey(window, keycode) == GLFW_PRESS;
}

void GLFWWindowImpl::AddKeyListener(std::function<void(int, int, int, int)> callback) {
    this->callback = callback;
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, StaticKeyCallback);
}

void GLFWWindowImpl::KeyCallback(const int key, const int scancode, const int action, const int mods) {
    callback(key, scancode, action, mods);
}

Vector2 GLFWWindowImpl::GetMousePosition() {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    return Vector2(x, y);
}

bool GLFWWindowImpl::GetKeyDown(const int keycode) {
    return glfwGetKey(window, keycode) == GLFW_PRESS;
}

void GLFWWindowImpl::StaticKeyCallback(GLFWwindow *window, const int key, const int scancode, const int action, const int mods) {
    auto* instance = static_cast<GLFWWindowImpl*>(glfwGetWindowUserPointer(window));

    if (instance) {
        instance->KeyCallback(key, scancode, action, mods);
    }
}
