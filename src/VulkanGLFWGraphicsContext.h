//
// Created by zykov on 6/13/2026.
//

#ifndef MAXENGINE_VULKANGLFWGRAPHICSCONTEXT_H
#define MAXENGINE_VULKANGLFWGRAPHICSCONTEXT_H
#include <vulkan/vulkan_core.h>

#include "IGLFWGraphicsContext.h"


class VulkanGLFWGraphicsContext : public IGLFWGraphicsContext {
public:
    ~VulkanGLFWGraphicsContext() override;
    void Init() override;
    void SetContext(IWindow* window) override;

private:
    VkInstance instance = nullptr;
    VkSurfaceKHR surface = nullptr;
};


#endif //MAXENGINE_VULKANGLFWGRAPHICSCONTEXT_H
