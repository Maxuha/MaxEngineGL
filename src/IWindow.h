//
// Created by zykov on 5/30/2026.
//

#ifndef MAXENGINE_IWINDOW_H
#define MAXENGINE_IWINDOW_H
#include "Scene.h"
#define VK_USE_PLATFORM_WIN32_KHR
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <vulkan/vulkan_core.h>

#include "IInputContext.h"


enum class GraphicsApi {
    Vulkan,
    OpenGL
};

struct WindowDesc {
    int width, height;
};

struct Rect;

class IWindow : public IInputContext {
public:
    ~IWindow() override = default;

    virtual void Close() const = 0;
    virtual bool IsOpen() const = 0;
    virtual double GetDeltaTime() = 0;
    virtual void* GetHandle() = 0;
    virtual void SetApi(GraphicsApi api) = 0;
    virtual void MakeOGLContext() = 0;
    virtual void* GetProcAddress() = 0;
    virtual bool CheckVulkanSupport() = 0;
    virtual const char **GetExtensions(uint32_t& count) = 0;
    virtual bool CreateSurface(VkInstance& instance, VkSurfaceKHR &surface) = 0;
    virtual HWND GetHWND() = 0;
    virtual Rect GetCurrentSize() = 0;
};


#endif //MAXENGINE_IWINDOW_H
