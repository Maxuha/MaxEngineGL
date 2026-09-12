//
// Created by zykov on 6/21/2026.
//

#include "VulkanTextureManager.h"

#include "../../gl/model/TextureData.h"
#include "../src/renderer/vulkan/vertexbuffer/VulkanBufferManager.h"
#include "../vertexbuffer/VulkanStagingBufferPool.h"


namespace Rendering {
    VulkanTextureManager::VulkanTextureManager(VulkanLogicalDevice &device, VulkanStagingBufferPool &stagingBufferPool,
                                               VulkanCommandBufferManager &commandBufferManager) : device(device),
        stagingBufferPool(stagingBufferPool), commandBufferManager(commandBufferManager) {
        textures.reserve(100);
    }

    VulkanTextureManager::~VulkanTextureManager() {
        for (const auto texture: textures) {
            vkDestroyImageView(device.GetDevice(), texture.ImageView, nullptr);
            vkDestroyImage(device.GetDevice(), texture.Image, nullptr);
            vkFreeMemory(device.GetDevice(), texture.DeviceMemory, nullptr);
            vkDestroySampler(device.GetDevice(), texture.Sampler, nullptr);
        }
    }

    VkSampler CreateSampler(const VulkanLogicalDevice &device) {
        VkSampler textureSampler;

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = 16;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        if (vkCreateSampler(device.GetDevice(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }

        return textureSampler;
    }

    TextureHandle VulkanTextureManager::CreateTexture(const size_t width, const size_t height, const TextureFormat format, void *data) {
        VkImage textureImage;
        VkDeviceMemory textureImageMemory;

        const VkFormat _format = ToVulkanFormat(format);
        VkImageUsageFlags usageFlags;
        VkImageAspectFlags aspectFlags;

        if (format == TextureFormat::DEPTH) {
            aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
        } else {
            aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        if (format == TextureFormat::DEPTH) {
            usageFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        } else {
            usageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        }

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = static_cast<uint32_t>(width);
        imageInfo.extent.height = static_cast<uint32_t>(height);
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = _format;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usageFlags;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.flags = 0; // Optional

        if (vkCreateImage(device.GetDevice(), &imageInfo, nullptr, &textureImage) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image!");
        }

        const VkMemoryRequirements memRequirements = device.GetMemoryRequirements(textureImage);
        const uint32_t memoryTypeIndex = device.GetPhysicalDevice().FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = memoryTypeIndex;

        if (vkAllocateMemory(device.GetDevice(), &allocInfo, nullptr, &textureImageMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(device.GetDevice(), textureImage, textureImageMemory, 0);

        VulkanTexture texture;
        texture.Image = textureImage;
        texture.ImageView = CreateImageView(textureImage, _format, aspectFlags);
        texture.Sampler = CreateSampler(device);
        texture.Width = width;
        texture.Height = height;
        texture.MipLevels = 1;

        textures.push_back(texture);

        return TextureHandle{.Id = textures.size() - 1};
    }

    VulkanTexture VulkanTextureManager::GetTexture(const TextureHandle texture) const {
        return textures[texture.Id];
    }

    void VulkanTextureManager::UploadData(const VkCommandBuffer cmd, const TextureHandle handle, const void *data, const size_t size) {
        const VulkanTexture texture = GetTexture(handle);
        const StagingAllocation staging = stagingBufferPool.Allocate(size);
        std::memcpy(staging.cpuMappedPointer, data, size);

        TransitionImageLayout(cmd, texture.Image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        CopyBufferToImage(cmd, staging.vkBuffer, staging.offset, texture.Image, texture.Width, texture.Height);
        TransitionImageLayout(cmd, texture.Image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);



        // vkQueueWaitIdle(static_cast<VulkanLogicalDevice *>(context.LogicalDevice)->GetGraphicsQueue());
        //vkFreeCommandBuffers(static_cast<VulkanLogicalDevice *>(context->LogicalDevice)->GetDevice(), commandPool, 1, &commandBuffer);

        stagingBufferPool.Reset();
    }

    VkImageView VulkanTextureManager::CreateImageView(const VkImage image, const VkFormat format,
                                                      const VkImageAspectFlags aspectFlags) const {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VkImageView imageView;

        if (vkCreateImageView(device.GetDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image view!");
        }

        return imageView;
    }

    void VulkanTextureManager::CopyBufferToImage(const VkCommandBuffer cmd, const VkBuffer buffer,
                                                 const VkDeviceSize offset, const VkImage image, const uint32_t width,
                                                 const uint32_t height) {
        VkBufferImageCopy region{};
        region.bufferOffset = offset;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
            width,
            height,
            1
        };

        vkCmdCopyBufferToImage(
            cmd,
            buffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
        );
    }

    void VulkanTextureManager::TransitionImageLayout(VkCommandBuffer cmd, const VkImage image, VkFormat format,
                                                     const VkImageLayout oldLayout,
                                                     const VkImageLayout newLayout) {
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = 0; // TODO
        barrier.dstAccessMask = 0; // TODO

        if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

            if (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT) {
                barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        } else {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout ==
                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout ==
                   VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                                    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        } else {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(
            cmd,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );
    }

    constexpr VkFormat VulkanTextureManager::ToVulkanFormat(const TextureFormat format) const {
        switch (format) {
            case TextureFormat::RGB:       return VK_FORMAT_R8G8B8_UNORM;
            case TextureFormat::RGBA:      return VK_FORMAT_R8G8B8A8_UNORM;
            case TextureFormat::SRGBA:     return VK_FORMAT_R8G8B8A8_SRGB;
            case TextureFormat::D32_FLOAT: return VK_FORMAT_D32_SFLOAT;
            case TextureFormat::DEPTH:     return VK_FORMAT_D32_SFLOAT_S8_UINT;
            default:                       return VK_FORMAT_UNDEFINED;
        }
    }
} // Rendering
