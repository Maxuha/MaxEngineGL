//
// Created by zykov on 7/8/2026.
//

#include "VulkanBufferManager.h"
#include "../VulkanFrameManager.h"
#include "../VulkanSwapChain.h"
#include "../../domain/buffer/BufferDesc.h"


namespace Rendering {
    VulkanBufferManager::VulkanBufferManager(
        VulkanLogicalDevice& logicalDevice,
        VulkanFrameManager& frameManager,
        VulkanCommandBufferManager& commandBufferManager
        ): logicalDevice(logicalDevice),
    frameManager(frameManager), commandBufferManager(commandBufferManager) {

    }

    VulkanBufferManager::~VulkanBufferManager() {
    }

    BufferHandle VulkanBufferManager::CreateBuffer(const BufferDesc& desc, const void *data) {
        constexpr int MAX_FRAMES_IN_FLIGHT = 3;

        VulkanVertexBuffer vulkanVertexBuffer;

        const size_t numAllocations = desc.isDynamic ? MAX_FRAMES_IN_FLIGHT : 1;

        vulkanVertexBuffer.buffers.resize(numAllocations);
        vulkanVertexBuffer.memories.resize(numAllocations);
        vulkanVertexBuffer.mappedPointers.resize(numAllocations, nullptr);
        vulkanVertexBuffer.descriptorSet = ResourceSetHandle {0};
        vulkanVertexBuffer.size = desc.size;

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
            case BufferUsage::Staging:
                vkUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                vkProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
                break;
        }

        if (desc.isDynamic) {
            for (size_t i = 0; i < numAllocations; i++) {
                CreateBuffer(desc.size, vkUsage, vkProperties, vulkanVertexBuffer.buffers[i], vulkanVertexBuffer.memories[i]);

                vkMapMemory(logicalDevice.GetDevice(), vulkanVertexBuffer.memories[i], 0, desc.size, 0, &vulkanVertexBuffer.mappedPointers[i]);

                if (data) {
                    memcpy(vulkanVertexBuffer.mappedPointers[i], data, desc.size);
                }
            }
        } else {
            CreateBuffer(desc.size, vkUsage, vkProperties, vulkanVertexBuffer.buffers[0], vulkanVertexBuffer.memories[0]);
        }

        vertexBuffers.push_back(vulkanVertexBuffer);

        return BufferHandle { vertexBuffers.size() - 1 };
    }

     VulkanBuffer VulkanBufferManager::CreateInternalBuffer(const BufferDesc& desc, const void *data) const {
        constexpr int MAX_FRAMES_IN_FLIGHT = 3;

        VulkanBuffer vulkanBuffer;

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
            case BufferUsage::Staging:
                vkUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                vkProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
                break;
        }

        if (desc.isDynamic) {
            for (size_t i = 0; i < numAllocations; i++) {
                CreateBuffer(desc.size, vkUsage, vkProperties, vulkanBuffer.buffer, vulkanBuffer.bufferMemory);

                vkMapMemory(logicalDevice.GetDevice(), vulkanBuffer.bufferMemory, 0, desc.size, 0, &vulkanBuffer.mappedPointer);

                if (data) {
                    memcpy(vulkanBuffer.mappedPointer, data, desc.size);
                }
            }
        } else {
            CreateBuffer(desc.size, vkUsage, vkProperties, vulkanBuffer.buffer, vulkanBuffer.bufferMemory);
        }
        return vulkanBuffer;
    }

    VkBuffer VulkanBufferManager::CreateStagingBuffer(const void *data, const size_t size) const {
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* _data;
        vkMapMemory(logicalDevice.GetDevice(), stagingBufferMemory, 0, size, 0, &_data);
        memcpy(_data, data, size);
        vkUnmapMemory(logicalDevice.GetDevice(), stagingBufferMemory);
        return stagingBuffer;
    }

    BufferHandle VulkanBufferManager::CreateVertexBuffer(const std::vector<Vertex> vertexes) {
        const VkDeviceSize bufferSize = vertexes.size() * sizeof(Vertex);

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(logicalDevice.GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertexes.data(), bufferSize);
        vkUnmapMemory(logicalDevice.GetDevice(), stagingBufferMemory);

        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;

        CreateBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

        CopyBuffer(stagingBuffer, vertexBuffer, bufferSize);

        // vkDestroyBuffer(static_cast<VulkanLogicalDevice *>(context.LogicalDevice)->GetDevice(), stagingBuffer, nullptr);
        // vkFreeMemory(static_cast<VulkanLogicalDevice *>(context.LogicalDevice)->GetDevice(), stagingBufferMemory, nullptr);

        VulkanVertexBuffer vulkanVertexBuffer;
        vulkanVertexBuffer.vertexBuffer = vertexBuffer;

        vertexBuffers.push_back(vulkanVertexBuffer);

        return BufferHandle { vertexBuffers.size() - 1 };
    }

    BufferHandle VulkanBufferManager::CreateIndexBuffer(const std::vector<uint16_t> indices) {
        const VkDeviceSize bufferSize = indices.size() * sizeof(uint16_t);

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(logicalDevice.GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), bufferSize);
        vkUnmapMemory(logicalDevice.GetDevice(), stagingBufferMemory);

        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;

        CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

        CopyBuffer(stagingBuffer, indexBuffer, bufferSize);

        // vkDestroyBuffer(static_cast<VulkanLogicalDevice *>(context.LogicalDevice)->GetDevice(), stagingBuffer, nullptr);
        // vkFreeMemory(static_cast<VulkanLogicalDevice *>(context.LogicalDevice)->GetDevice(), stagingBufferMemory, nullptr);

        VulkanVertexBuffer vulkanVertexBuffer;
        vulkanVertexBuffer.vertexBuffer = indexBuffer;

        vertexBuffers.push_back(vulkanVertexBuffer);

        return BufferHandle { vertexBuffers.size() - 1 };
    }

    BufferHandle VulkanBufferManager::CreateUniformBuffer(void *data, const size_t size) {
        constexpr int MAX_FRAMES_IN_FLIGHT = 3;

        VulkanVertexBuffer vulkanVertexBuffer;
        vulkanVertexBuffer.buffers.resize(MAX_FRAMES_IN_FLIGHT);
        vulkanVertexBuffer.memories.resize(MAX_FRAMES_IN_FLIGHT);
        vulkanVertexBuffer.mappedPointers.resize(MAX_FRAMES_IN_FLIGHT);
        vulkanVertexBuffer.size = size;

        const VkDeviceSize bufferSize = size;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vulkanVertexBuffer.buffers[i], vulkanVertexBuffer.memories[i]);

            vkMapMemory(logicalDevice.GetDevice(), vulkanVertexBuffer.memories[i], 0, bufferSize, 0, &  vulkanVertexBuffer.mappedPointers[i]);
        }

        vertexBuffers.push_back(vulkanVertexBuffer);

        return BufferHandle { vertexBuffers.size() - 1 };
    }

    void VulkanBufferManager::UpdateUniformBuffer(const BufferHandle handle, const void* data, const size_t offset, const size_t size) {
        const uint32_t currentFrame = frameManager.GetCurrentIndexFrame();
        const VulkanVertexBuffer& vulkanVertexBuffer = vertexBuffers[handle.Id];
        const auto dstMemory = static_cast<uint8_t*>(vulkanVertexBuffer.mappedPointers[currentFrame]);
        memcpy(dstMemory + offset, data, size);
    }

    void VulkanBufferManager::UpdateInternalUniformBuffer(VulkanVertexBuffer &buffer, const void *data, const size_t offset,
        const size_t size) const {
        const uint32_t currentFrame = frameManager.GetCurrentIndexFrame();
        const auto dstMemory = static_cast<uint8_t*>(buffer.mappedPointers[currentFrame]);
        memcpy(dstMemory + offset, data, size);
    }

    VulkanVertexBuffer * VulkanBufferManager::GetVertexBuffer(const BufferHandle handle) {
        return &vertexBuffers[handle.Id];
    }

    void VulkanBufferManager::BindDescriptorSet(const BufferHandle buffer_handle, const ResourceSetHandle set_handle) {
        vertexBuffers[buffer_handle.Id].descriptorSet = set_handle;
    }

    ResourceSetHandle VulkanBufferManager::GetDescriptorSet(const BufferHandle buffer_handle) const {
        return vertexBuffers[buffer_handle.Id].descriptorSet;
    }

    void VulkanBufferManager::CreateBuffer(const VkDeviceSize size, const VkBufferUsageFlags usage,
                                           const VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory) const {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(logicalDevice.GetDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vertex buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(logicalDevice.GetDevice(), buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = logicalDevice.GetPhysicalDevice().FindMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(logicalDevice.GetDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate vertex buffer memory!");
        }

        vkBindBufferMemory(logicalDevice.GetDevice(), buffer, bufferMemory, 0);
    }

    void VulkanBufferManager::CopyBuffer(const VkBuffer srcBuffer, const VkBuffer dstBuffer, const VkDeviceSize size) const {
        const VkCommandBuffer commandBuffer = commandBufferManager.AllocateCommandBuffer(CommandPoolType::Transfer, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

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
       // vkQueueWaitIdle(logicalDevice.GetGraphicsQueue());

        //vkFreeCommandBuffers(logicalDevice.GetDevice(), commandPoolManager.GetCommandPool(CommandPoolHandle{.Id = 0}).CommandPool, 1, &commandBuffer);
    }
} // Rendering