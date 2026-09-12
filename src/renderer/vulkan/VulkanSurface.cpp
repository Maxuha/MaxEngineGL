//
// Created by zykov on 6/17/2026.
//

#include "VulkanSurface.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <vulkan/vulkan_core.h>
#include "VulkanInstance.h"
#include "../src/IWindow.h"


namespace Rendering {
    VulkanSurface::VulkanSurface(VulkanInstance &instance, IWindow &window) : instance(&instance), window(&window)  {
        VkWin32SurfaceCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hwnd = window.GetHWND();
        createInfo.hinstance = GetModuleHandle(nullptr);

        if (vkCreateWin32SurfaceKHR(instance.GetInstance(), &createInfo, nullptr, &surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    VulkanSurface::~VulkanSurface() {
        vkDestroySurfaceKHR(instance->GetInstance(), surface, nullptr);
    }

    VkSurfaceKHR VulkanSurface::GetSurface() const {
        return surface;
    }
} // Rendering