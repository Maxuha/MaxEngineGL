//
// Created by zykov on 6/19/2026.
//

#include "VulkanShaderManager.h"
#include "../VulkanLogicalDevice.h"
#include <vulkan/vulkan_core.h>

namespace Rendering {
    VulkanShaderManager::VulkanShaderManager(VulkanLogicalDevice& device) : device(&device) {
    }

    VulkanShaderManager::~VulkanShaderManager() {
    }

    ShaderHandle VulkanShaderManager::CreateShader(const ShaderType type, const std::vector<char> &code) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;
        if (const VkResult result = vkCreateShaderModule(device->GetDevice(), &createInfo, nullptr, &shaderModule);
            result != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }

        const ShaderHandle handle = { .Id = shaders.size()};

        shaders.push_back(VulkanShader { .VkShader = shaderModule, .Type = type });

        return handle;
    }

    ShaderHandle VulkanShaderManager::CreateShader(ShaderType type, const char *code) {
        return ShaderHandle {};
    }

    VulkanShader VulkanShaderManager::CreateShader(const std::vector<char> &code) const {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;

        if (const VkResult result = vkCreateShaderModule(device->GetDevice(), &createInfo, nullptr, &shaderModule);
            result != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
            }

        return VulkanShader { .VkShader = shaderModule};
    }

    VulkanShader VulkanShaderManager::GetShader(const ShaderHandle handle) const {
        return shaders[handle.Id];
    }
} // Rendering