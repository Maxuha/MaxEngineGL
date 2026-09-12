//
// Created by zykov on 6/17/2026.
//

#ifndef MAXENGINE_VULKANSURFACE_H
#define MAXENGINE_VULKANSURFACE_H
#include <vulkan/vulkan_core.h>

#include "../ISurface.h"
class IWindow;

namespace Rendering {
    class VulkanInstance;

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    class VulkanSurface : public ISurface {
    public:
        VulkanSurface(VulkanInstance& instance, IWindow &window);

        ~VulkanSurface() override;

        VkSurfaceKHR GetSurface() const;

    private:
        VkSurfaceKHR surface;
        VulkanInstance* instance;
        IWindow* window;
    };
} // Rendering

#endif //MAXENGINE_VULKANSURFACE_H
