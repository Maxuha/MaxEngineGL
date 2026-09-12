//
// Created by zykov on 7/8/2026.
//

#ifndef MAXENGINE_VERTEXBUFFER_H
#define MAXENGINE_VERTEXBUFFER_H

#include "../../domain//IBufferManager.h"
#include "../../IResourceSetManager.h"

namespace Rendering {
    class VulkanLogicalDevice;
    class VulkanFrameManager;
    class VulkanCommandBufferManager;

    struct VulkanVertexBuffer {
        std::vector<VkBuffer> buffers;
        std::vector<VkDeviceMemory> memories;
        std::vector<void*> mappedPointers;
        VkBuffer vertexBuffer;
        uint32_t size;
        ResourceSetHandle descriptorSet;
    };

    struct VulkanBuffer {
        VkBuffer buffer;
        VkDeviceMemory bufferMemory;
        void* mappedPointer;
    };

    class VulkanBufferManager : public IBufferManager {
    public:
        explicit VulkanBufferManager(
            VulkanLogicalDevice& logicalDevice,
            VulkanFrameManager& frameManager,
            VulkanCommandBufferManager& commandBufferManager
        );

        ~VulkanBufferManager() override;

        BufferHandle CreateBuffer(const BufferDesc& desc, const void* data) override;

        VulkanBuffer CreateInternalBuffer(const BufferDesc& desc, const void* data) const;

        VkBuffer CreateStagingBuffer(const void* data, const size_t size) const;

        BufferHandle CreateVertexBuffer(std::vector<Vertex> vertexes) override;

        BufferHandle CreateIndexBuffer(std::vector<uint16_t> indices) override;

        BufferHandle CreateUniformBuffer(void *data, size_t size) override;

        void UpdateUniformBuffer(BufferHandle handle, const void* data, size_t offset, size_t size) override;

        void UpdateInternalUniformBuffer(VulkanVertexBuffer& buffer, const void* data, size_t offset, size_t size) const;

        VulkanVertexBuffer* GetVertexBuffer(BufferHandle handle);

        void BindDescriptorSet(BufferHandle buffer_handle, ResourceSetHandle set_handle);

        ResourceSetHandle GetDescriptorSet(BufferHandle buffer_handle) const;

    private:
        VulkanLogicalDevice& logicalDevice;
        VulkanFrameManager& frameManager;
        VulkanCommandBufferManager& commandBufferManager;

        std::vector<VulkanVertexBuffer> vertexBuffers;
        std::vector<VulkanBuffer> buffers;

        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) const;

        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;
    };
} // Rendering

#endif //MAXENGINE_VERTEXBUFFER_H
