//
// Created by zykov on 6/17/2026.
//

#ifndef MAXENGINE_VULKANINSTANCE_H
#define MAXENGINE_VULKANINSTANCE_H

#include <vulkan/vulkan_core.h>

#include "../IInstance.h"

class IWindow;

namespace Rendering {
    class VulkanInstance : public IInstance {
    public:
        explicit VulkanInstance(IWindow& window);
        ~VulkanInstance() override;

        VkInstance GetInstance() const;

    private:
        VkInstance instance = VK_NULL_HANDLE;

        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    };
} // Rendering

#endif //MAXENGINE_VULKANINSTANCE_H
