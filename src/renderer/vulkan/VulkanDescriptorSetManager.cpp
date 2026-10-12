//
// Created by zykov on 7/22/2026.
//

#include "VulkanDescriptorSetManager.h"

#include "VulkanLogicalDevice.h"

namespace Rendering {
    VulkanDescriptorSetManager::VulkanDescriptorSetManager(const VulkanLogicalDevice& device) : device(device) {

    }

    VulkanDescriptorSetManager::~VulkanDescriptorSetManager() {
    }

    ResourceSetLayoutHandle VulkanDescriptorSetManager::CreateDescriptorSetLayout(
        const ResourceSetLayoutDesc &resourceSetLayoutDesc) {

        auto it = layoutHandles.find(resourceSetLayoutDesc);
        if (it != layoutHandles.end()) {
            return it->second;
        }

        std::vector<VkDescriptorSetLayoutBinding> vkBindings;
        vkBindings.reserve(resourceSetLayoutDesc.bindings.size());

        for (const auto &binding: resourceSetLayoutDesc.bindings) {
            VkDescriptorSetLayoutBinding vkBinding{};
            vkBinding.binding = binding.binding_slot;
            vkBinding.descriptorCount = resourceSetLayoutDesc.bindings.size();

            switch (binding.type) {
                case ResourceType::SampledImage:
                    vkBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    break;
                case ResourceType::UniformBuffer:
                    vkBinding.descriptorType = binding.is_dynamic
                                                   ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
                                                   : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    break;
                case ResourceType::StorageBuffer:
                    vkBinding.descriptorType = binding.is_dynamic
                                                   ? VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC
                                                   : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                    break;
                case ResourceType::StorageImage:
                    vkBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                    break;
                case ResourceType::Sampler:
                    vkBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
                    break;
            }

            vkBinding.stageFlags = 0;
            const uint32_t stages = static_cast<uint32_t>(binding.stage_flags);

            if (stages & static_cast<uint32_t>(ShaderStageFlags::Vertex)) {
                vkBinding.stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
            }
            if (stages & static_cast<uint32_t>(ShaderStageFlags::Fragment)) {
                vkBinding.stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
            }
            if (stages & static_cast<uint32_t>(ShaderStageFlags::Compute)) {
                vkBinding.stageFlags |= VK_SHADER_STAGE_COMPUTE_BIT;
            }
            if (stages == static_cast<uint32_t>(ShaderStageFlags::AllStages)) {
                vkBinding.stageFlags = VK_SHADER_STAGE_ALL;
            }

            vkBinding.pImmutableSamplers = nullptr;

            vkBindings.push_back(vkBinding);
        }

        VkDescriptorSetLayout descriptorSetLayout;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = vkBindings.size();
        layoutInfo.pBindings = vkBindings.data();

        if (vkCreateDescriptorSetLayout(device.GetDevice(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }

        layouts.push_back(descriptorSetLayout);

        const ResourceSetLayoutHandle handle = {.Id = layouts.size() - 1};

        layoutHandles[resourceSetLayoutDesc] = handle;

        return handle;
    }

    DescriptorPoolHandle VulkanDescriptorSetManager::CreateDescriptorPool(const uint32_t poolSize) {
        std::array<VkDescriptorPoolSize, 2> poolSizes{};

        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = poolSize;

        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = poolSize;

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = poolSize;

        VkDescriptorPool vulkanDescriptorPool;

        if (vkCreateDescriptorPool(device.GetDevice(), &poolInfo, nullptr, &vulkanDescriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }

        pools.push_back(vulkanDescriptorPool);

        return DescriptorPoolHandle(pools.size() - 1);
    }

    VkDescriptorPool VulkanDescriptorSetManager::AllocateDescriptorPool() {
        if (currentIndex % maxIndexPerPool == 0) {
            CreateDescriptorPool(maxIndexPerPool);
        }

        const VkDescriptorPool currentPool = pools[currentIndex / maxIndexPerPool];
        currentIndex += 1;

        return currentPool;
    }

    std::vector<VkDescriptorSet> VulkanDescriptorSetManager::CreateDescriptorSets(const VkDescriptorSetLayout layout, const uint32_t count) {
        const VkDescriptorPool descriptorPool = AllocateDescriptorPool();

        const std::vector layouts(count, layout);

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size()); ;
        allocInfo.pSetLayouts = layouts.data();

        std::vector<VkDescriptorSet> vulkanDescriptorSet(count);

        if (const VkResult result = vkAllocateDescriptorSets(device.GetDevice(), &allocInfo, vulkanDescriptorSet.data()); result != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets! Error code: " + std::to_string(result));
        }

        return vulkanDescriptorSet;
    }

    VulkanDescriptorSet VulkanDescriptorSetManager::GetDescriptorSet(const ResourceSetHandle setHandle) {
        return sets[setHandle.id];
    }

    VkDescriptorSetLayout VulkanDescriptorSetManager::GetDescriptorSetLayout(const ResourceSetLayoutHandle layoutHandle) const {
        return layouts[layoutHandle.Id];
    }

    void VulkanDescriptorSetManager::BindBuffer(const ResourceSetHandle setHandle, uint32_t slot, const BufferHandle bufferHandle) {

    }

    void VulkanDescriptorSetManager::BindTexture(const ResourceSetHandle setHandle, uint32_t slot, const TextureHandle textureHandle) {

    }
} // Rendering