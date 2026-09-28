//
// Created by zykov on 6/16/2026.
//

#include "VulkanRenderDevice.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <vulkan/vulkan_core.h>

#include "stb_image_write.h"
#include "VulkanDescriptorSetManager.h"
#include "VulkanFrameManager.h"
#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanSurface.h"
#include "VulkanSwapChain.h"
#include "commandbuffer/VulkanCommandBufferManager.h"
#include "framebuffer/VulkanFrameBufferManager.h"
#include "pipeline/VulkanPipelineManager.h"
#include "shaders/VulkanShaderManager.h"
#include "texture/VulkanTextureManager.h"
#include "vertexbuffer/VulkanBufferManager.h"


namespace Rendering {

#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

// ??????????????? ??????? ??? ?????? ??????????? ???? ??????
uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    return 0xFFFFFFFF;
}

bool SaveVkImageToDisk(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkCommandPool commandPool,
    VkQueue queue,
    VkImage srcImage,
    VkFormat format,
    uint32_t width,
    uint32_t height,
    const char* filename)
{
    if (srcImage == VK_NULL_HANDLE || width == 0 || height == 0) {
        std::cerr << "Error: Invalid image or dimensions (" << width << "x" << height << ")\n";
        return false;
    }

    bool isDepth = false;
    int channels = 4;
    size_t elementSize = 1;

    if (format == VK_FORMAT_D32_SFLOAT || format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D16_UNORM) {
        isDepth = true;
        channels = 1;
        elementSize = (format == VK_FORMAT_D16_UNORM) ? 2 : 4;
    } else if (format == VK_FORMAT_R8G8B8_UNORM || format == VK_FORMAT_R8G8B8_SRGB) {
        channels = 3;
        elementSize = 1;
    } else if (format == VK_FORMAT_R8G8B8A8_UNORM || format == VK_FORMAT_R8G8B8A8_SRGB || format == VK_FORMAT_B8G8R8A8_UNORM || format == VK_FORMAT_B8G8R8A8_SRGB) {
        channels = 4;
        elementSize = 1;
    } else {
        std::cerr << "Error: Unsupported image format for saving.\n";
        return false;
    }

    VkDeviceSize bufferSize = static_cast<VkDeviceSize>(width) * height * channels * elementSize;

    VkBuffer dstBuffer;
    VkDeviceMemory dstBufferMemory;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &dstBuffer) != VK_SUCCESS) {
        std::cerr << "Error: Failed to create buffer.\n";
        return false;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, dstBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &dstBufferMemory) != VK_SUCCESS) {
        std::cerr << "Error: Failed to allocate buffer memory.\n";
        vkDestroyBuffer(device, dstBuffer, nullptr);
        return false;
    }
    vkBindBufferMemory(device, dstBuffer, dstBufferMemory, 0);

    VkCommandBufferAllocateInfo cmdAllocInfo{};
    cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdAllocInfo.commandPool = commandPool;
    cmdAllocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &cmdAllocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    bool hasStencil = (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D16_UNORM_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = srcImage;

    if (isDepth) {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        if (hasStencil) {
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    } else {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = isDepth ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT; // ????????? DEPTH
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, 1};

    vkCmdCopyImageToBuffer(commandBuffer, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstBuffer, 1, &region);

    // ?????????? Image ? ???????? ??????
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    vkEndCommandBuffer(commandBuffer);

    // ?????????? ??????? ?? ?????????? ? ???? ??????????
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);
    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);

    // 4. ?????? ?????? ?????? ?? CPU ? ?????????????? ?????? ????????
    void* data;
    vkMapMemory(device, dstBufferMemory, 0, bufferSize, 0, &data);

    std::vector<unsigned char> pixels;

    if (isDepth) {
        pixels.resize(static_cast<size_t>(width) * height);
        float minDepth = std::numeric_limits<float>::max();
        float maxDepth = std::numeric_limits<float>::lowest();

        // ????????? ???????? ??????? (?????????????? 32-bit float ??????)
        float* depthData = reinterpret_cast<float*>(data);
        size_t pixelCount = static_cast<size_t>(width) * height;

        for (size_t i = 0; i < pixelCount; ++i) {
            float value = depthData[i];
            if (!std::isfinite(value)) continue;
            minDepth = std::min(minDepth, value);
            maxDepth = std::max(maxDepth, value);
        }

        const bool hasRange = minDepth < maxDepth;
        const float range = hasRange ? (maxDepth - minDepth) : 1.0f;

        for (size_t i = 0; i < pixelCount; ++i) {
            float rawValue = std::clamp(depthData[i], 0.0f, 1.0f);
            float normalized = hasRange ? ((rawValue - minDepth) / range) : rawValue;
            pixels[i] = static_cast<unsigned char>(std::clamp(normalized, 0.0f, 1.0f) * 255.0f);
        }
        std::cout << "Depth image stats for " << filename << ": min=" << minDepth << ", max=" << maxDepth << "\n";
    } else {
        // ??? ???????? ????? (RGB/RGBA)
        pixels.resize(static_cast<size_t>(width) * height * channels);
        auto srcData = static_cast<unsigned char*>(data);

        // ???? ?????? BGR(A), ???????????? ? RGB(A) ??? stb_image
        if (format == VK_FORMAT_B8G8R8A8_UNORM || format == VK_FORMAT_B8G8R8A8_SRGB) {
            for (size_t i = 0; i < static_cast<size_t>(width) * height; ++i) {
                pixels[i * channels + 0] = srcData[i * channels + 2]; // R <- B
                pixels[i * channels + 1] = srcData[i * channels + 1]; // G <- G
                pixels[i * channels + 2] = srcData[i * channels + 0]; // B <- R
                if (channels == 4) pixels[i * channels + 3] = srcData[i * channels + 3]; // A
            }
        } else {
            std::memcpy(pixels.data(), srcData, bufferSize);
        }
    }

    vkUnmapMemory(device, dstBufferMemory);
    vkDestroyBuffer(device, dstBuffer, nullptr);
    vkFreeMemory(device, dstBufferMemory, nullptr);

    // 5. ?????? ? ???? ? ??????? stb_image_write
    stbi_flip_vertically_on_write(true);
    int success = stbi_write_png(filename, width, height, channels, pixels.data(), width * channels);

    if (!success) {
        std::cerr << "Error: Failed to write image file: " << filename << "\n";
        return false;
    }

    std::cout << "Successfully saved Vulkan image (" << width << "x" << height << ") to " << filename << "\n";
    return true;
}


    //

    VulkanRenderDevice::VulkanRenderDevice(IWindow &window) : physicalDevice(nullptr),
                                                              logicalDevice(nullptr),
                                                              frameBufferManager(nullptr),
                                                              commandPoolManager(nullptr),
                                                              textureManager(nullptr),
                                                              bufferManager(nullptr),
                                                              queueFamily(nullptr),
                                                              renderPass(nullptr) {
        if (!window.CheckVulkanSupport()) {
            std::cout << "Vulkan is not supported" << std::endl;
            return;
        }

        MAX_FRAMES_IN_FLIGHT = 3;

        instance = new VulkanInstance(window);

        surface = new VulkanSurface(*instance, window);

        physicalDevice = new VulkanPhysicalDevice(*instance, *surface);

        logicalDevice = new VulkanLogicalDevice(*physicalDevice);

        shaderManager = new VulkanShaderManager(*logicalDevice);

        pipelineManager = new VulkanPipelineManager(*logicalDevice, *shaderManager);

        commandBufferManager = new VulkanCommandBufferManager(*logicalDevice);

        stagingBufferPool = new VulkanStagingBufferPool(logicalDevice->GetDevice(), physicalDevice->GetPhysicalDevice(),
                                                        512 * 1024 * 1024);

        renderPassManager = new VulkanRenderPassManager(*logicalDevice);

        frameBufferManager = new VulkanFrameBufferManager(*logicalDevice, *renderPassManager, *textureManager);

        // swapchain
        swapChain = new VulkanSwapChain(
            *physicalDevice,
            *logicalDevice,
            *surface,
            window,
            *frameBufferManager,
            *renderPassManager
        );


        swapChain->CreateTextures();
        swapChain->CreateFrameBuffers();

        // end swapchain

        frameManager = new VulkanFrameManager(
            *logicalDevice,
            *swapChain,
            *frameBufferManager,
            *commandBufferManager
        );

        bufferManager = new VulkanBufferManager(*logicalDevice, *frameManager, *commandBufferManager);

        textureManager = new VulkanTextureManager(*logicalDevice, *stagingBufferPool, *commandBufferManager);

        descriptorSetManager = new VulkanDescriptorSetManager(*logicalDevice);

        const Rect rect = window.GetCurrentSize();

        const TextureHandle texture = textureManager->CreateTexture(rect.width, rect.height, TextureFormat::DEPTH, nullptr);
        depthTexture = textureManager->GetTexture(texture);
    }

    VulkanRenderDevice::~VulkanRenderDevice() {
    }

    CommandBuffer *VulkanRenderDevice::AllocateCommandBuffer() {
        return new CommandBuffer(2048 * 2048);
    }

    TextureHandle VulkanRenderDevice::CreateTexture(const TextureCreateRequest &desc, const void *initialData) {
        const auto data = const_cast<void *>(initialData);

        const TextureHandle textureHandle = textureManager->CreateTexture(desc.Width, desc.Height, desc.Format, data);

        if (initialData != nullptr) {
            const VkCommandBuffer uploadCmd = commandBufferManager->AllocateCommandBuffer(
                CommandPoolType::Transfer, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

            VkCommandBufferBeginInfo beginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            vkBeginCommandBuffer(uploadCmd, &beginInfo);

            const size_t size = desc.Width * desc.Height * 4;
            textureManager->UploadData(uploadCmd, textureHandle, data, size);

            vkEndCommandBuffer(uploadCmd);

            VkSubmitInfo submitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO};
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &uploadCmd;

            vkQueueSubmit(logicalDevice->GetTransferQueue(), 1, &submitInfo, VK_NULL_HANDLE);
        }

        return textureHandle;
    }

    PipelineLayoutHandle VulkanRenderDevice::CreatePipelineLayout(
        const std::vector<ResourceSetLayoutHandle> &resourceSetLayouts) {
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts;

        for (const auto resourceSetLayout: resourceSetLayouts) {
            descriptorSetLayouts.push_back(descriptorSetManager->GetDescriptorSetLayout(resourceSetLayout));
        }

        return pipelineManager->CreatePipelineLayout(descriptorSetLayouts);
    }

    ResourceSetLayoutHandle VulkanRenderDevice::CreateResourceSetLayout(
        const ResourceSetLayoutDesc &resourceSetLayoutDesc) {
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

        return descriptorSetManager->CreateDescriptorSetLayout(vkBindings);
    }

    ResourceSetHandle VulkanRenderDevice::CreateResourceSet(const ResourceSetLayoutHandle resourceSetLayoutHandle) {
        const VkDescriptorSetLayout descriptorSetLayout = descriptorSetManager->GetDescriptorSetLayout(
            resourceSetLayoutHandle);
        const std::vector<VkDescriptorSet> descriptorSets = descriptorSetManager->CreateDescriptorSets(
            descriptorSetLayout, MAX_FRAMES_IN_FLIGHT);
        VulkanDescriptorSet vulkanDescriptorSet;
        vulkanDescriptorSet.set = descriptorSets;
        this->descriptorSets.push_back(vulkanDescriptorSet);
        return ResourceSetHandle{.id = this->descriptorSets.size() - 1};
    }

    void VulkanRenderDevice::UpdateResourceSet(const ResourceSetHandle setHandle, const uint32_t binding,
                                               const TextureHandle textureHandle) {
        const VulkanDescriptorSet resourceSet = descriptorSets[setHandle.id];
        const VulkanTexture texture = textureManager->GetTexture(textureHandle);

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = texture.ImageView;
            imageInfo.sampler = texture.Sampler;

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = resourceSet.set[i];
            descriptorWrite.dstBinding = binding;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pImageInfo = &imageInfo;

            std::array descriptorWrites = {descriptorWrite};

            vkUpdateDescriptorSets(logicalDevice->GetDevice(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
        }
    }

    void VulkanRenderDevice::UpdateResourceSet(const ResourceSetHandle setHandle, const uint32_t binding,
                                               const BufferHandle bufferHandle) {
        const VulkanDescriptorSet descriptorSet = descriptorSets[setHandle.id];
        const VulkanVertexBuffer buffer = buffers[bufferHandle.Id];

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = buffer.buffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = buffer.size;

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = descriptorSet.set[i];
            descriptorWrite.dstBinding = binding;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfo;
            descriptorWrite.pImageInfo = nullptr; // Optional
            descriptorWrite.pTexelBufferView = nullptr; // Optional

            std::array descriptorWrites = {descriptorWrite};

            vkUpdateDescriptorSets(logicalDevice->GetDevice(), descriptorWrites.size(), descriptorWrites.data(), 0,
                                   nullptr);
        }
    }

    BufferHandle VulkanRenderDevice::CreateBuffer(const BufferDesc &desc, const void *data) {
        VulkanVertexBuffer vbuffer;
        vbuffer.size = desc.size;

        if (desc.isDynamic) {
            vbuffer.buffers.resize(MAX_FRAMES_IN_FLIGHT);
            vbuffer.memories.resize(MAX_FRAMES_IN_FLIGHT);
            vbuffer.mappedPointers.resize(MAX_FRAMES_IN_FLIGHT);
            for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                const VulkanBuffer buffer = bufferManager->CreateInternalBuffer(desc, data);
                vbuffer.buffers[i] = buffer.buffer;
                vbuffer.memories[i] = buffer.bufferMemory;
                vbuffer.mappedPointers[i] = buffer.mappedPointer;
            }
        } else {
            vbuffer.buffers.resize(1);
            vbuffer.memories.resize(1);
            vbuffer.mappedPointers.resize(1);

            const VulkanBuffer buffer = bufferManager->CreateInternalBuffer(desc, data);
            vbuffer.buffers[0] = buffer.buffer;
            vbuffer.memories[0] = buffer.bufferMemory;
            vbuffer.mappedPointers[0] = buffer.mappedPointer;
        }

        if (!desc.isDynamic) {
            const StagingAllocation stagingBuffer = stagingBufferPool->Allocate(desc.size);
            memcpy(stagingBuffer.cpuMappedPointer, data, desc.size);

            const VkCommandBuffer commandBuffer = commandBufferManager->AllocateCommandBuffer(
                CommandPoolType::Transfer, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            vkBeginCommandBuffer(commandBuffer, &beginInfo);

            VkBufferCopy copyRegion{};
            copyRegion.srcOffset = 0; // Optional
            copyRegion.dstOffset = 0; // Optional
            copyRegion.size = desc.size;

            vkCmdCopyBuffer(commandBuffer, stagingBuffer.vkBuffer, vbuffer.buffers[0], 1, &copyRegion);

            vkEndCommandBuffer(commandBuffer);

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffer;

            vkQueueSubmit(logicalDevice->GetTransferQueue(), 1, &submitInfo, VK_NULL_HANDLE);
            vkQueueWaitIdle(logicalDevice->GetTransferQueue());

            stagingBufferPool->Reset();
        }

        buffers.push_back(vbuffer);

        return BufferHandle{.Id = buffers.size() - 1};
    }

    void VulkanRenderDevice::UpdateBuffer(const BufferHandle id, const void *data, const int offset, const int size) {
        VulkanVertexBuffer &buffer = buffers[id.Id];
        bufferManager->UpdateInternalUniformBuffer(buffer, data, offset, size);
    }

    void VulkanRenderDevice::DestroyBuffer(BufferHandle id) {
    }

    PipelineHandle VulkanRenderDevice::CreatePipeline(const ShaderDesc &shaderDesc,
                                                      const PipelineStateDesc &pipelineStateDesc) {
        const VulkanShader vShader = shaderManager->CreateShader(shaderDesc.vertexCode);
        const VulkanShader fShader = shaderManager->CreateShader(shaderDesc.fragmentCode);

        GraphicsVulkanShader vulkanShader;
        vulkanShader.VertexShader = vShader;
        vulkanShader.FragmentShader = fShader;

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
        descriptorSetLayouts.reserve(shaderDesc.resourceSetLayouts.size());

        for (auto resource_set_layout: shaderDesc.resourceSetLayouts) {
            const ResourceSetLayoutHandle resource_set_layout_handle = CreateResourceSetLayout(resource_set_layout);
            descriptorSetLayouts.push_back(descriptorSetManager->GetDescriptorSetLayout(resource_set_layout_handle));
        }
        vulkanShader.DescriptorSetLayouts = descriptorSetLayouts;

        shaders.push_back(vulkanShader);

        const auto shaderHandle = ShaderHandle{.Id = shaders.size() - 1};
        const GraphicsVulkanShader shader = shaders[shaderHandle.Id];
        return pipelineManager->CreatePipeline(shader, pipelineStateDesc);;
    }

    void VulkanRenderDevice::SubmitCommandBuffer(CommandBuffer *commandBuffer) {
        VulkanFrame &frame = frameManager->AcquireFrame();
        frame.DepthImageView = depthTexture.ImageView;

        const VkCommandBuffer vkCommandBuffer = commandBufferManager->AllocateCommandBuffer(
            CommandPoolType::Graphics, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
         VkCommandBuffer vkSecondaryCommandBuffer = commandBufferManager->AllocateCommandBuffer(
            CommandPoolType::Graphics, VK_COMMAND_BUFFER_LEVEL_SECONDARY);

        const VkCommandBuffer vkSecondaryCommandBuffer2 = commandBufferManager->AllocateCommandBuffer(
            CommandPoolType::Graphics, VK_COMMAND_BUFFER_LEVEL_SECONDARY);

        bool isFirstPassCompleted = false;

        frame.commandBuffer = vkCommandBuffer;
        frame.secondaryCommandBuffer = vkSecondaryCommandBuffer;

        vkResetCommandBuffer(vkCommandBuffer, 0);
        vkResetCommandBuffer(vkSecondaryCommandBuffer, 0);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;

        vkBeginCommandBuffer(vkCommandBuffer, &beginInfo);

        std::vector<uint8_t> commands = commandBuffer->GetCommands();
        size_t readOffset = 0;
        const size_t head = commandBuffer->GetHead();

        auto commandBufferi = VulkanCommandBufferi(vkCommandBuffer);

        VkImage depthImage = nullptr;

        VkImage lastRenderTargetTexture = nullptr;

        while (readOffset < head) {
            const GLCommandType type = *reinterpret_cast<GLCommandType *>(&commands[readOffset]);
            readOffset += sizeof(GLCommandType);
            switch (type) {
                case GLCommandType::BeginRenderPass: {
                    const auto *cmd = reinterpret_cast<GLCommand_BeginRenderPass *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_BeginRenderPass);

                    if (isFirstPassCompleted) {
                        vkSecondaryCommandBuffer = vkSecondaryCommandBuffer2;
                    }

                    Color color = cmd->clearColor;
                    Rect viewport = cmd->viewport;

                    std::optional<AttachmentDescription> colorAttachmentDescription = cmd->colorAttachment;
                    std::optional<AttachmentDescription> depthAttachmentDescription = cmd->depthAttachment;

                    VkCommandBufferInheritanceRenderingInfo inheritanceRenderingInfo{};
                    inheritanceRenderingInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_RENDERING_INFO;
                    inheritanceRenderingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

                    if (colorAttachmentDescription.has_value()) {
                        VkFormat colorFormat = VK_FORMAT_B8G8R8A8_SRGB;

                        inheritanceRenderingInfo.colorAttachmentCount = 1;
                        inheritanceRenderingInfo.pColorAttachmentFormats = &colorFormat;
                    }

                    if (depthAttachmentDescription.has_value()) {
                        inheritanceRenderingInfo.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT;
                    }

                    VkCommandBufferInheritanceInfo inheritanceInfo{};
                    inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
                    inheritanceInfo.pNext = &inheritanceRenderingInfo;

                    VkCommandBufferBeginInfo secondaryBeginInfo{};
                    secondaryBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                    secondaryBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
                    secondaryBeginInfo.pInheritanceInfo = &inheritanceInfo;

                    std::vector<VkRenderingAttachmentInfo> colorAttachments;

                    if (colorAttachmentDescription.has_value()) {
                        VkRenderingAttachmentInfo colorAttachment{};
                        colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
                        colorAttachment.imageView = frame.ColorTexture.ImageView; // <-- Render directly to the view
                        colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                        colorAttachment.clearValue.color = {color.r, color.g, color.b, color.a};
                        colorAttachments.push_back(colorAttachment);
                    }

                    VkRenderingAttachmentInfo vkDepthAttachment{};
                    if (depthAttachmentDescription.has_value()) {
                        VkImageView depthImageView = frame.DepthImageView;

                        VkRenderingAttachmentInfo depthAttachment{};
                        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

                        if (depthAttachmentDescription.value().renderTarget.Id != 999999) {
                            depthImageView = textureManager->GetTexture(depthAttachmentDescription.value().renderTarget).ImageView;
                            depthImage = textureManager->GetTexture(depthAttachmentDescription.value().renderTarget).Image;

                            commandBufferi.transitionImageLayout(depthImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_DEPTH_BIT, 1, 1);

                            depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                        }

                        depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
                        depthAttachment.imageView = depthImageView; // <-- Render directly to the view
                        depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
                        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                        depthAttachment.clearValue.depthStencil.depth = 1.0f;
                        depthAttachment.clearValue.depthStencil.stencil = 0;
                        vkDepthAttachment = depthAttachment;
                    }

                    VkRect2D renderArea = {
                        {static_cast<int32_t>(viewport.x), static_cast<int32_t>(viewport.y)},
                        {static_cast<uint32_t>(viewport.width), static_cast<uint32_t>(viewport.height)}
                    };

                    uint32_t colorAttachmentCount;
                    VkRenderingAttachmentInfo *pColorAttachments;

                    if (colorAttachments.size() > 0) {
                        colorAttachmentCount = colorAttachments.size();
                        pColorAttachments = colorAttachments.data();
                    } else {
                        colorAttachmentCount = 0;
                        pColorAttachments = nullptr;
                    }

                    VkRenderingInfo renderingInfo{};
                    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
                    renderingInfo.flags = VK_RENDERING_CONTENTS_SECONDARY_COMMAND_BUFFERS_BIT;
                    renderingInfo.renderArea = renderArea;
                    renderingInfo.layerCount = 1;
                    renderingInfo.colorAttachmentCount = colorAttachmentCount;
                    renderingInfo.pColorAttachments = pColorAttachments;
                    renderingInfo.pDepthAttachment = &vkDepthAttachment;
                    renderingInfo.pStencilAttachment = nullptr;

                    vkCmdBeginRendering(vkCommandBuffer, &renderingInfo);
                    vkBeginCommandBuffer(vkSecondaryCommandBuffer, &secondaryBeginInfo);
                    break;
                }
                case GLCommandType::BindPipeline: {
                    const auto *cmd = reinterpret_cast<GLCommand_BindPipeline *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_BindPipeline);

                    VulkanPipeline vkPipeline = pipelineManager->GetPipeline(cmd->pipeline);

                    vkCmdBindPipeline(vkSecondaryCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline.Pipeline);
                    break;
                }
                case GLCommandType::UpdateBuffer: {
                    const auto *cmd = reinterpret_cast<GLCommand_UpdateBuffer *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_UpdateBuffer);

                    auto bufferHandle = cmd->buffer;
                    auto data = cmd->data;
                    auto offset = cmd->offset;
                    auto size = cmd->size;

                    const uint32_t currentFrame = frameManager->GetCurrentIndexFrame();
                    const VulkanVertexBuffer *vulkanVertexBuffer = bufferManager->GetVertexBuffer(bufferHandle);
                    const auto dstMemory = static_cast<uint8_t *>(vulkanVertexBuffer->mappedPointers[currentFrame]);
                    memcpy(dstMemory + offset, data, size);
                    break;
                }
                case GLCommandType::UpdateUniformBuffer: {
                    const auto *cmd = reinterpret_cast<GLCommand_UpdateBuffer *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_UpdateBuffer);

                    auto bufferHandle = cmd->buffer;
                    auto data = cmd->data;
                    auto offset = cmd->offset;
                    auto size = cmd->size;

                    const uint32_t currentFrame = frameManager->GetCurrentIndexFrame();
                    const VulkanVertexBuffer &vulkanVertexBuffer = buffers[bufferHandle.Id];
                    const auto dstMemory = static_cast<uint8_t *>(vulkanVertexBuffer.mappedPointers[currentFrame]);
                    memcpy(dstMemory + offset, data, size);
                    break;
                }
                case GLCommandType::BindVertexBuffer: {
                    const auto *cmd = reinterpret_cast<GLCommand_BindBuffer *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_BindBuffer);

                    BufferHandle bufferHandle = cmd->buffer;
                    VulkanVertexBuffer &buffer = buffers[bufferHandle.Id];

                    VkBuffer vertexBuffers[] = {buffer.buffers[0]};
                    VkDeviceSize offsets[] = {0};

                    vkCmdBindVertexBuffers(vkSecondaryCommandBuffer, 0, 1, vertexBuffers, offsets);
                    break;
                }
                case GLCommandType::BindIndexBuffer: {
                    const auto *cmd = reinterpret_cast<GLCommand_BindBuffer *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_BindBuffer);

                    BufferHandle bufferHandle = cmd->buffer;
                    VulkanVertexBuffer &buffer = buffers[bufferHandle.Id];

                    VkBuffer indexBuffer = buffer.buffers[0];
                    VkDeviceSize offset = 0;

                    vkCmdBindIndexBuffer(vkSecondaryCommandBuffer, indexBuffer, offset, VK_INDEX_TYPE_UINT16);
                    break;
                }
                case GLCommandType::BindResourceSet: {
                    const auto *cmd = reinterpret_cast<GLCommand_BindResourceSet *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_BindResourceSet);

                    auto resourceSet = descriptorSets[cmd->resourceId.id];
                    const std::vector<VkDescriptorSet> descriptorSets = resourceSet.set;
                    const auto imageIndex = swapChain->GetCurrentImageIndex();

                    if (cmd->isPipelineLayout) {
                        vkCmdBindDescriptorSets(
                            vkSecondaryCommandBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipelineManager->GetPipelineLayout(cmd->pipelineLayout),
                            cmd->index,
                            1,
                            &descriptorSets[imageIndex],
                            0,
                            nullptr
                        );
                    } else {
                        vkCmdBindDescriptorSets(
                            vkSecondaryCommandBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipelineManager->GetPipeline(cmd->pipeline).pipelineLayout,
                            cmd->index,
                            1,
                            &descriptorSets[imageIndex],
                            0,
                            nullptr
                        );
                    }
                    break;
                }
                case GLCommandType::Draw: {
                    const auto *cmd = reinterpret_cast<GLCommand_Draw *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_Draw);

                    vkCmdDraw(vkSecondaryCommandBuffer, cmd->vertexCount, 1, 0, 0);
                    break;
                }
                case GLCommandType::DrawIndexed: {
                    const auto *cmd = reinterpret_cast<GLCommand_DrawIndexed *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_DrawIndexed);

                    vkCmdDrawIndexed(vkSecondaryCommandBuffer, cmd->indexCount, 1, 0, 0, 0);
                    break;
                }
                case GLCommandType::EndRenderPass: {
                    const auto *cmd = reinterpret_cast<GLCommand_EndRenderPass *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_EndRenderPass);

                    vkEndCommandBuffer(vkSecondaryCommandBuffer);

                    vkCmdExecuteCommands(vkCommandBuffer, 1, &vkSecondaryCommandBuffer);

                    vkCmdEndRendering(vkCommandBuffer);

                    if (depthImage != nullptr) {
                        commandBufferi.transitionImageLayout(depthImage, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_DEPTH_BIT, 1, 1);
                        lastRenderTargetTexture = depthImage;
                        depthImage = nullptr;
                    } else {
                        commandBufferi.transitionImageLayout(frame.ColorTexture.Image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_ASPECT_COLOR_BIT, 1, 1);
                    }

                    isFirstPassCompleted = true;
                    break;
                }
                default: ;
            }
        }

        vkEndCommandBuffer(vkCommandBuffer);

        frameManager->SendFrameToGPU(frame);

    if (lastRenderTargetTexture != nullptr) {
        VkCommandPool commandPool;

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = 0;

        if (const VkResult result = vkCreateCommandPool(logicalDevice->GetDevice(), &poolInfo, nullptr, &commandPool);
            result != VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool!");
            }

         SaveVkImageToDisk(logicalDevice->GetDevice(), physicalDevice->GetPhysicalDevice(), commandPool, logicalDevice->GetGraphicsQueue(), lastRenderTargetTexture, VK_FORMAT_D32_SFLOAT, 2160, 1440, "output_texture2.png");
        lastRenderTargetTexture = nullptr;
    }

        frameManager->Present(frame);
    }

    void VulkanRenderDevice::Present() {
    }

    TextureFormat VulkanRenderDevice::GetDisplayFormat() {
        return MapVkFormat(swapChain->GetImageFormat());
    }
} // Rendering
