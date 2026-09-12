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
        const std::vector<VkDescriptorSetLayoutBinding> &bindings) {
        VkDescriptorSetLayout descriptorSetLayout;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = bindings.size();
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(device.GetDevice(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }

        layouts.push_back(descriptorSetLayout);

        return ResourceSetLayoutHandle { .Id = layouts.size() - 1 };
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
        // constexpr int MAX_FRAMES_IN_FLIGHT = 3;
        //
        // const VulkanVertexBuffer* buffer = static_cast<VulkanBufferManager *>(context.BufferManager)->GetVertexBuffer(bufferHandle);
        // VulkanDescriptorSet& descriptorSet = sets[setHandle.id];
        //
        // for (int i  = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        //     VkDescriptorBufferInfo bufferInfo{};
        //     bufferInfo.buffer = buffer->buffers[i];
        //     bufferInfo.offset = 0;
        //     bufferInfo.range = buffer->size;
        //
        //     VkWriteDescriptorSet descriptorWrite{};
        //     descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        //     descriptorWrite.dstSet = descriptorSet.set[i];
        //     descriptorWrite.dstBinding = descriptorSet.binding;
        //     descriptorWrite.dstArrayElement = 0;
        //     descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        //     descriptorWrite.descriptorCount = 1;
        //     descriptorWrite.pBufferInfo = &bufferInfo;
        //     descriptorWrite.pImageInfo = nullptr; // Optional
        //     descriptorWrite.pTexelBufferView = nullptr; // Optional
        //
        //     std::array descriptorWrites = { descriptorWrite };
        //
        //     vkUpdateDescriptorSets(device.GetDevice(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
        // }
    }

    void VulkanDescriptorSetManager::BindTexture(const ResourceSetHandle setHandle, uint32_t slot, const TextureHandle textureHandle) {
        // constexpr int MAX_FRAMES_IN_FLIGHT = 3;
        //
        // const VulkanTexture texture = static_cast<VulkanTextureManager *>(context.TextureManager)->GetTexture(textureHandle);
        //
        // for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        //     VkDescriptorImageInfo imageInfo{};
        //     imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        //     imageInfo.imageView = texture.ImageView;
        //     imageInfo.sampler = texture.Sampler;
        //
        //     VulkanDescriptorSet& descriptorSet = sets[setHandle.id];
        //
        //     VkWriteDescriptorSet descriptorWrite{};
        //     descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        //     descriptorWrite.dstSet = descriptorSet.set[i];
        //     descriptorWrite.dstBinding = descriptorSet.binding;
        //     descriptorWrite.dstArrayElement = 0;
        //     descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        //     descriptorWrite.descriptorCount = 1;
        //     descriptorWrite.pImageInfo = &imageInfo;
        //
        //     std::array descriptorWrites = { descriptorWrite };
        //
        //     vkUpdateDescriptorSets(device.GetDevice(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
        // }
    }
} // Rendering