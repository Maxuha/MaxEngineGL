//
// Created by zykov on 7/8/2026.
//

#ifndef MAXENGINE_VERTEXBUFFER_H
#define MAXENGINE_VERTEXBUFFER_H

#include <vulkan/vulkan_core.h>

#include "../../IResourceSetManager.h"
#include "../../domain/buffer/BufferUsage.h"

namespace Rendering {
    struct BufferDesc;
    enum class BufferUsage;
    class VulkanLogicalDevice;
    class VulkanFrameManager;
    class VulkanCommandBufferManager;

    struct VulkanDynamicBuffer {
        std::vector<VkBuffer> buffers;
        std::vector<VkDeviceMemory> memories;
        std::vector<void*> mappedPointers;
        uint32_t size;
        ResourceSetHandle descriptorSet;
        bool isDynamic = false;
        BufferUsage usage = BufferUsage::Vertex;

        void resize(const uint32_t count) {
            buffers.resize(count);
            memories.resize(count);
            mappedPointers.resize(count);
        }
    };

    struct VulkanBuffer {
        VkBuffer buffer;
        VkDeviceMemory memory;
        void* mappedPointer;
        uint32_t size;
    };

    class VulkanBufferManager {
    public:
        explicit VulkanBufferManager(
            VulkanLogicalDevice& logicalDevice,
            VulkanCommandBufferManager& commandBufferManager
        );

        ~VulkanBufferManager();

        VulkanBuffer CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) const;

        VulkanBuffer CreateInternalBuffer(const BufferDesc& desc) const;

        VulkanBuffer CreateStagingBuffer(const void* data, size_t size) const;

        VulkanBuffer CreateVertexBuffer(const void* data, size_t size) const;

        VulkanBuffer CreateIndexBuffer(const void* data, size_t size) const;

        VulkanBuffer CreateUniformBuffer(void *data, size_t size) const;

        VulkanBuffer CreateDeviceLocalBuffer(VkDeviceSize size, VkBufferUsageFlags usage) const;

        VulkanBuffer CreateHostVisibleBuffer(VkDeviceSize size, VkBufferUsageFlags usage) const;

        VulkanDynamicBuffer* GetVertexBuffer(BufferHandle handle);

        void BindDescriptorSet(BufferHandle buffer_handle, ResourceSetHandle set_handle);

        ResourceSetHandle GetDescriptorSet(BufferHandle buffer_handle) const;

    private:
        VulkanLogicalDevice& logicalDevice;
        VulkanCommandBufferManager& commandBufferManager;

        std::vector<VulkanDynamicBuffer> vertexBuffers;
        std::vector<VulkanBuffer> buffers;

        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) const;

        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;
    };
} // Rendering

#endif //MAXENGINE_VERTEXBUFFER_H
