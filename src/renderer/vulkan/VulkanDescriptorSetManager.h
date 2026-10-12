//
// Created by zykov on 7/22/2026.
//

#ifndef MAXENGINE_VULKANDESCRIPTORSETMANAGER_H
#define MAXENGINE_VULKANDESCRIPTORSETMANAGER_H

#include <vulkan/vulkan_core.h>

#include "VulkanLogicalDevice.h"
#include "../IResourceSetManager.h"


namespace Rendering {
    struct ResourceSetLayoutDescHash;

    struct VulkanDescriptorSet {
        std::vector<VkDescriptorSet> set;
    };

    class VulkanDescriptorSetManager : public IResourceSetManager {
    public:
        explicit VulkanDescriptorSetManager(const VulkanLogicalDevice& device);

        ~VulkanDescriptorSetManager() override;

        ResourceSetLayoutHandle CreateDescriptorSetLayout(const ResourceSetLayoutDesc &resourceSetLayoutDesc);

        std::vector<VkDescriptorSet> CreateDescriptorSets(VkDescriptorSetLayout layout, uint32_t count);

        void BindBuffer(ResourceSetHandle setHandle, uint32_t slot, BufferHandle bufferHandle) override;

        void BindTexture(ResourceSetHandle setHandle, uint32_t slot, TextureHandle textureHandle) override;

        VulkanDescriptorSet GetDescriptorSet(ResourceSetHandle setHandle);

        VkDescriptorSetLayout GetDescriptorSetLayout(ResourceSetLayoutHandle layoutHandle) const;

    private:
        VulkanLogicalDevice device;

        std::vector<VkDescriptorPool> pools;
        std::vector<VulkanDescriptorSet> sets;
        std::vector<VkDescriptorSetLayout> layouts;
        std::unordered_map<ResourceSetLayoutDesc, ResourceSetLayoutHandle, ResourceSetLayoutDescHash> layoutHandles;

        uint32_t currentIndex = 0;
        uint32_t maxIndexPerPool = 2000;

        DescriptorPoolHandle CreateDescriptorPool(uint32_t poolSize);

        VkDescriptorPool AllocateDescriptorPool();
    };
} // Rendering

#endif //MAXENGINE_VULKANDESCRIPTORSETMANAGER_H
