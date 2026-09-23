//
// Created by zykov on 3/17/2026.
//

#ifndef MAXENGINE_WINDOW_H
#define MAXENGINE_WINDOW_H

#include <vulkan/vulkan_core.h>
#include "IGLFWGraphicsContext.h"
#include "IWindow.h"
#include "Scene.h"
#include "GLFW/glfw3.h"

class GLFWWindowImpl : public IWindow, public Rendering::IWindowContext {
public:
    explicit GLFWWindowImpl(const WindowDesc& desc, IGLFWGraphicsContext* graphicsContext);

    ~GLFWWindowImpl() override;

    void Close() const override;

    bool IsOpen() const override;

    double GetDeltaTime() override;

    void * GetNativeHandle() override;

    void* GetHandle() override;

    void SetApi(GraphicsApi api) override;

    void MakeOGLContext() override;

    void * GetProcAddress() override;

    bool CheckVulkanSupport() override;

    const char **GetExtensions(uint32_t& count) override;

    bool CreateSurface(VkInstance& instance, VkSurfaceKHR &surface) override;

    HWND GetHWND() override;

    Rect GetCurrentSize() override;

    bool isKeyPressed(int keycode) override;

    void AddKeyListener(std::function<void(int, int, int, int)> callback) override;

    void KeyCallback(int key, int scancode, int action, int mods);

    Vector2 GetMousePosition() override;

    bool GetKeyDown(int keycode) override;

private:

    int width, height;
    float aspectRatio;

    double last_time = glfwGetTime();
    double delta_time = 0;

    static void StaticKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    std::function<void(int, int, int, int)> callback;

protected:
    GLFWwindow *window = nullptr;
};


#endif //MAXENGINE_WINDOW_H