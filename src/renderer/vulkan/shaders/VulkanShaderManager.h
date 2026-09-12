//
// Created by zykov on 6/19/2026.
//

#ifndef MAXENGINE_SHADERMANAGER_H
#define MAXENGINE_SHADERMANAGER_H
#include <vulkan/vulkan_core.h>
#include "../../managers/IShaderManager.h"

namespace Rendering {

    class VulkanLogicalDevice;

    struct VulkanShader {
        VkShaderModule VkShader;
        ShaderType Type;
    };

    struct GraphicsVulkanShader {
        VulkanShader VertexShader;
        VulkanShader FragmentShader;
        std::vector<VkDescriptorSetLayout> DescriptorSetLayouts;
    };

    class VulkanShaderManager : public IShaderManager {
    public:
        explicit VulkanShaderManager(VulkanLogicalDevice &device);

        ~VulkanShaderManager() override;

        ShaderHandle CreateShader(ShaderType type, const std::vector<char> &code) override;

        ShaderHandle CreateShader(ShaderType type, const char *code) override;

        VulkanShader CreateShader(const std::vector<char> &code) const;

        VulkanShader GetShader(ShaderHandle handle) const;

    private:
        VulkanLogicalDevice *device;

        std::vector<VulkanShader> shaders;
    };
} // Rendering

#endif //MAXENGINE_SHADERMANAGER_H
