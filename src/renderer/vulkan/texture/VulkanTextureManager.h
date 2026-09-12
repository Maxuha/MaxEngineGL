//
// Created by zykov on 6/21/2026.
//

#ifndef MAXENGINE_TEXTUREMANAGER_H
#define MAXENGINE_TEXTUREMANAGER_H
#include <vulkan/vulkan_core.h>

#include "../../ITextureManager.h"
#include "../src/renderer/vulkan/VulkanLogicalDevice.h"
#include "../commandbuffer/VulkanCommandBufferManager.h"
#include "../vertexbuffer/VulkanStagingBufferPool.h"

namespace Rendering {
    struct VulkanTexture {
        VkImage Image;
        VkImageView ImageView;
        VkDeviceMemory DeviceMemory;
        VkSampler Sampler;
        uint32_t Width;
        uint32_t Height;
        uint32_t MipLevels;
    };

    class VulkanTextureManager : public ITextureManager {
    public:
        explicit VulkanTextureManager(VulkanLogicalDevice& device, VulkanStagingBufferPool& stagingBufferPool, VulkanCommandBufferManager& commandBufferManager);

        ~VulkanTextureManager() override;

        TextureHandle CreateTexture(size_t width, size_t height, TextureFormat format, void *data) override;

        VulkanTexture GetTexture(TextureHandle texture) const;

        void UploadData(VkCommandBuffer cmd, TextureHandle handle, const void *data, size_t size);

    private:
        VulkanLogicalDevice& device;
        VulkanStagingBufferPool& stagingBufferPool;
        VulkanCommandBufferManager& commandBufferManager;

        std::vector<VulkanTexture> textures;

        VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) const;
        void CopyBufferToImage(VkCommandBuffer cmd, VkBuffer buffer, VkDeviceSize offset, VkImage image, uint32_t width, uint32_t height) ;
        void TransitionImageLayout(VkCommandBuffer cmd, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

        constexpr VkFormat ToVulkanFormat(TextureFormat format) const;
    };
} // Rendering

#endif //MAXENGINE_TEXTUREMANAGER_H
