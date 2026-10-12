//
// Created by zykov on 7/8/2026.
//

#include "VulkanBufferManager.h"
#include "../VulkanFrameManager.h"
#include "../../domain/buffer/BufferDesc.h"


namespace Rendering {
    VulkanBufferManager::VulkanBufferManager(
        VulkanLogicalDevice &logicalDevice,
        VulkanCommandBufferManager &commandBufferManager
    ) : logicalDevice(logicalDevice), commandBufferManager(commandBufferManager) {
    }

    VulkanBufferManager::~VulkanBufferManager() {
    }

    VulkanBuffer VulkanBufferManager::CreateBuffer(const VkDeviceSize size, const VkBufferUsageFlags usage,
                                                   const VkMemoryPropertyFlags properties) const {
        VulkanBuffer outBuffer;
        outBuffer.size = size;

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(logicalDevice.GetDevice(), &bufferInfo, nullptr, &outBuffer.buffer) != VK_SUCCESS) {
            throw std::runtime_error("VulkanBufferManager: failed to create VkBuffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(logicalDevice.GetDevice(), outBuffer.buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(logicalDevice.GetDevice(), &allocInfo, nullptr, &outBuffer.memory) != VK_SUCCESS) {
            throw std::runtime_error("VulkanBufferManager: failed to allocate VkDeviceMemory!");
        }

        vkBindBufferMemory(logicalDevice.GetDevice(), outBuffer.buffer, outBuffer.memory, 0);

        return outBuffer;
    }

    VulkanBuffer VulkanBufferManager::CreateInternalBuffer(const BufferDesc &desc) const {
        constexpr int MAX_FRAMES_IN_FLIGHT = 3;

        VulkanBuffer vulkanBuffer;
        vulkanBuffer.size = desc.size;

        const size_t numAllocations = desc.isDynamic ? MAX_FRAMES_IN_FLIGHT : 1;

        VkBufferUsageFlags vkUsage = 0;
        VkMemoryPropertyFlags vkProperties = 0;

        switch (desc.usage) {
            case BufferUsage::Vertex:
                vkUsage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
                vkProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
                break;
            case BufferUsage::Index:
                vkUsage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
                vkProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
                break;
            case BufferUsage::Uniform:
                vkUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
                vkProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
                break;
            case BufferUsage::Storage:
                vkUsage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
                vkProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
                break;
        }

        if (desc.isDynamic) {
            for (size_t i = 0; i < numAllocations; i++) {
                CreateBuffer(desc.size, vkUsage, vkProperties, vulkanBuffer.buffer, vulkanBuffer.memory);

                vkMapMemory(logicalDevice.GetDevice(), vulkanBuffer.memory, 0, desc.size, 0,
                            &vulkanBuffer.mappedPointer);
            }
        } else {
            CreateBuffer(desc.size, vkUsage, vkProperties, vulkanBuffer.buffer, vulkanBuffer.memory);
        }
        return vulkanBuffer;
    }

    VulkanBuffer VulkanBufferManager::CreateStagingBuffer(const void *data, const size_t size) const {
        return CreateBuffer(size,
                            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    }

    VulkanBuffer VulkanBufferManager::CreateVertexBuffer(const void *data, const size_t size) const {
        return CreateBuffer(size,
                            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    }

    VulkanBuffer VulkanBufferManager::CreateIndexBuffer(const void *data, const size_t size) const {
        return CreateBuffer(size,
                            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    }

    VulkanBuffer VulkanBufferManager::CreateUniformBuffer(void *data, const size_t size) const {
        return CreateBuffer(size,
                            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    }

    VulkanBuffer VulkanBufferManager::CreateDeviceLocalBuffer(const VkDeviceSize size, const VkBufferUsageFlags usage) const {
        return CreateBuffer(size, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    }

    VulkanBuffer VulkanBufferManager::CreateHostVisibleBuffer(const VkDeviceSize size, const VkBufferUsageFlags usage) const {
        return CreateBuffer(size, usage, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    }

    VulkanDynamicBuffer *VulkanBufferManager::GetVertexBuffer(const BufferHandle handle) {
        return &vertexBuffers[handle.Id];
    }

    void VulkanBufferManager::BindDescriptorSet(const BufferHandle buffer_handle, const ResourceSetHandle set_handle) {
        vertexBuffers[buffer_handle.Id].descriptorSet = set_handle;
    }

    ResourceSetHandle VulkanBufferManager::GetDescriptorSet(const BufferHandle buffer_handle) const {
        return vertexBuffers[buffer_handle.Id].descriptorSet;
    }

    uint32_t VulkanBufferManager::FindMemoryType(const uint32_t typeFilter,
                                                 const VkMemoryPropertyFlags properties) const {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(logicalDevice.GetPhysicalDevice().GetPhysicalDevice(), &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }
        throw std::runtime_error("VulkanBufferManager: ?? ??????? ????? ?????????? ??? ??????!");
    }

    void VulkanBufferManager::CreateBuffer(const VkDeviceSize size, const VkBufferUsageFlags usage,
                                           const VkMemoryPropertyFlags properties, VkBuffer &buffer,
                                           VkDeviceMemory &bufferMemory) const {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(logicalDevice.GetDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate  vertex buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(logicalDevice.GetDevice(), buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = logicalDevice.GetPhysicalDevice().FindMemoryType(
            memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(logicalDevice.GetDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate vertex buffer memory!");
        }

        vkBindBufferMemory(logicalDevice.GetDevice(), buffer, bufferMemory, 0);
    }

    void VulkanBufferManager::CopyBuffer(const VkBuffer srcBuffer, const VkBuffer dstBuffer,
                                         const VkDeviceSize size) const {
        const VkCommandBuffer commandBuffer = commandBufferManager.AllocateCommandBuffer(
            CommandPoolType::Transfer, VK_COMMAND_BUFFER_LEVEL_PRIMARY).GetCommandBuffer();

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0; // Optional
        copyRegion.dstOffset = 0; // Optional
        copyRegion.size = size;

        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(logicalDevice.GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    }
} // Rendering
