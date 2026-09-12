//
// Created by zykov on 6/13/2026.
//

#include "VulkanGLFWGraphicsContext.h"
#include <vulkan/vulkan_core.h>
#include <iostream>
#include "IWindow.h"
#include "GLFW/glfw3.h"

VulkanGLFWGraphicsContext::~VulkanGLFWGraphicsContext() {
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
};

void VulkanGLFWGraphicsContext::Init() {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

void VulkanGLFWGraphicsContext::SetContext(IWindow* window) {
    if (!glfwVulkanSupported()) {
        std::cout << "Vulkan is not supported" << std::endl;
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan App";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
    createInfo.enabledLayerCount = 0;

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("??????????? ??????: ?? ??????? ??????? VkInstance!");
    }

    if (glfwCreateWindowSurface(instance, static_cast<GLFWwindow*>(window->GetHandle()), nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("??????????? ??????: ?? ??????? ??????? ??????????? (Surface)!");
    }
}
