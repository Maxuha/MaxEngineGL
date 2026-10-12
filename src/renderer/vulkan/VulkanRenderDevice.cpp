//
// Created by zykov on 6/16/2026.
//

#include "VulkanRenderDevice.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <vulkan/vulkan_core.h>

#include "VulkanDescriptorSetManager.h"
#include "VulkanFrameManager.h"
#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanSurface.h"
#include "VulkanSwapChain.h"
#include "../ICommandPoolManager.h"
#include "../domain/buffer/BufferDesc.h"
#include "commandbuffer/VulkanCommandBufferManager.h"
#include "pipeline/VulkanPipelineManager.h"
#include "shaders/VulkanShaderManager.h"
#include "texture/VulkanTextureManager.h"
#include "vertexbuffer/VulkanBufferManager.h"


namespace Rendering {
    VulkanRenderDevice::VulkanRenderDevice(IWindow &window)
    {
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

        swapChain = new VulkanSwapChain(
            *physicalDevice,
            *logicalDevice,
            *surface,
            window
        );

        frameManager = new VulkanFrameManager(
            *logicalDevice,
            *swapChain,
            MAX_FRAMES_IN_FLIGHT
        );

        bufferManager = new VulkanBufferManager(*logicalDevice, *commandBufferManager);

        textureManager = new VulkanTextureManager(*logicalDevice, *stagingBufferPool, *commandBufferManager);

        descriptorSetManager = new VulkanDescriptorSetManager(*logicalDevice);

        const Rect rect = window.GetCurrentSize();

        const TextureHandle texture = textureManager->CreateTexture(rect.width, rect.height, TextureFormat::DEPTH, nullptr);
        depthTexture = textureManager->GetTexture(texture);

        mainCommandBuffer = commandBufferManager->AllocateCommandBuffer(CommandPoolType::Graphics, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        loaderCommandBuffer = commandBufferManager->AllocateCommandBuffer(CommandPoolType::Transfer, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    }

    VulkanRenderDevice::~VulkanRenderDevice() {
    }

    CommandBuffer *VulkanRenderDevice::AllocateCommandBuffer(const CommandBufferType type = CommandBufferType::Graphics) {
        // should be allicate here

        return new CommandBuffer(9048 * 1024, type);
    }

    TextureHandle VulkanRenderDevice::CreateTexture(const TextureCreateRequest &desc, const void *initialData) {
        const auto data = const_cast<void *>(initialData);

        const TextureHandle textureHandle = textureManager->CreateTexture(desc.Width, desc.Height, desc.Format, data);

        if (initialData != nullptr) {
            const VkCommandBuffer uploadCmd = commandBufferManager->AllocateCommandBuffer(
                CommandPoolType::Transfer, VK_COMMAND_BUFFER_LEVEL_PRIMARY).GetCommandBuffer();

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
        return descriptorSetManager->CreateDescriptorSetLayout(resourceSetLayoutDesc);
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

            vkUpdateDescriptorSets(logicalDevice->GetDevice(), descriptorWrites.size(), descriptorWrites.data(), 0,
                                   nullptr);
        }
    }

    void VulkanRenderDevice::UpdateResourceSet(const ResourceSetHandle setHandle, const uint32_t binding,
                                               const BufferHandle bufferHandle) {
        const VulkanDescriptorSet descriptorSet = descriptorSets[setHandle.id];
        const VulkanDynamicBuffer buffer = dynamicBuffers[bufferHandle.Id];

        VkDescriptorType descriptorType;

        if (buffer.isDynamic) {
            descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        } else {
            descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }

        if (buffer.usage == BufferUsage::Storage) {
            descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        }

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
            descriptorWrite.descriptorType = descriptorType;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfo;
            descriptorWrite.pImageInfo = nullptr; // Optional
            descriptorWrite.pTexelBufferView = nullptr; // Optional

            std::array descriptorWrites = {descriptorWrite};

            vkUpdateDescriptorSets(logicalDevice->GetDevice(), descriptorWrites.size(), descriptorWrites.data(), 0,
                                   nullptr);
        }
    }

    BufferHandle VulkanRenderDevice::CreateBuffer(const BufferDesc& desc) {
        VulkanDynamicBuffer vbuffer;
        vbuffer.size = desc.size;
        vbuffer.isDynamic = desc.isDynamicB;
        vbuffer.usage = desc.usage;

        if (desc.isNewApproach) {
            switch (desc.usage) {
                case BufferUsage::Vertex: {
                    const VulkanBuffer vertexBuffer = bufferManager->CreateDeviceLocalBuffer(desc.size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

                    buffers.push_back(vertexBuffer);

                    return BufferHandle { .Id = buffers.size() - 1 };
                }
                case BufferUsage::Index: {
                    const VulkanBuffer indexBuffer = bufferManager->CreateDeviceLocalBuffer(desc.size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

                    buffers.push_back(indexBuffer);

                    return BufferHandle { .Id = buffers.size() - 1 };
                }
                case BufferUsage::Uniform: {
                    VulkanDynamicBuffer buffers;
                    buffers.size = desc.size;
                    buffers.resize(MAX_FRAMES_IN_FLIGHT);

                    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                        VulkanBuffer buffer = bufferManager->CreateDeviceLocalBuffer(desc.size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

                        buffers.buffers[i] = buffer.buffer;
                        buffers.memories[i] = buffer.memory;

                        vkMapMemory(logicalDevice->GetDevice(), buffer.memory, 0, desc.size, 0, &buffers.mappedPointers[i]);
                    }

                    dynamicBuffers.push_back(buffers);

                    return BufferHandle { .Id = dynamicBuffers.size() - 1 };
                }
                case BufferUsage::Storage:
                    break;
            }
        }

        if (desc.isDynamic) {
            vbuffer.buffers.resize(MAX_FRAMES_IN_FLIGHT);
            vbuffer.memories.resize(MAX_FRAMES_IN_FLIGHT);
            vbuffer.mappedPointers.resize(MAX_FRAMES_IN_FLIGHT);
            for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                const VulkanBuffer temp = bufferManager->CreateInternalBuffer(desc);
                vbuffer.buffers[i] = temp.buffer;
                vbuffer.memories[i] = temp.memory;
                vbuffer.mappedPointers[i] = temp.mappedPointer;
            }
        } else {
            vbuffer.buffers.resize(1);
            vbuffer.memories.resize(1);
            vbuffer.mappedPointers.resize(1);

            const VulkanBuffer temp = bufferManager->CreateInternalBuffer(desc);
            vbuffer.buffers[0] = temp.buffer;
            vbuffer.memories[0] = temp.memory;
            vbuffer.mappedPointers[0] = temp.mappedPointer;
        }

        if (!desc.isDynamic) {
            const StagingAllocation stagingBuffer = stagingBufferPool->Allocate(desc.size);

            const VkCommandBuffer commandBuffer = commandBufferManager->AllocateCommandBuffer(
                CommandPoolType::Transfer, VK_COMMAND_BUFFER_LEVEL_PRIMARY).GetCommandBuffer();
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

        dynamicBuffers.push_back(vbuffer);

        return BufferHandle{.Id = dynamicBuffers.size() - 1};
    }

    void VulkanRenderDevice::UpdateBuffer(const BufferHandle id, const void *data, const int offset, const int size) {
        const uint32_t frameIndex = frameManager->GetCurrentIndexFrame();
        const VulkanDynamicBuffer &buffer = dynamicBuffers[id.Id];
        const auto dstMemory = static_cast<uint8_t*>(buffer.mappedPointers[frameIndex]);
        memcpy(dstMemory + offset, data, size);
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
        VulkanFrame frame;

        VulkanCommandBufferi mainCommandBuffer;

        if (commandBuffer->GetType() == CommandBufferType::Graphics) {
            frame = frameManager->AcquireFrame();
            mainCommandBuffer = this->mainCommandBuffer;
        } else if (commandBuffer->GetType() == CommandBufferType::Compute) {
            mainCommandBuffer = commandBufferManager->AllocateCommandBuffer(CommandPoolType::Compute, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        } else if (commandBuffer->GetType() == CommandBufferType::Transfer) {
            mainCommandBuffer = this->loaderCommandBuffer;
        }

        mainCommandBuffer.Begin();

        VulkanCommandBufferi secondaryCommandBuffer;

        std::vector<uint8_t> commands = commandBuffer->GetCommands();
        size_t readOffset = 0;
        const size_t head = commandBuffer->GetHead();

        VkImage depthImage = nullptr;

        while (readOffset < head) {
            const GLCommandType type = *reinterpret_cast<GLCommandType *>(&commands[readOffset]);
            readOffset += sizeof(GLCommandType);
            switch (type) {
                case GLCommandType::BeginRenderPass: {
                    const auto *cmd = reinterpret_cast<GLCommand_BeginRenderPass *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_BeginRenderPass);

                    secondaryCommandBuffer = commandBufferManager->AllocateCommandBuffer(CommandPoolType::Graphics, VK_COMMAND_BUFFER_LEVEL_SECONDARY);

                    Rect viewport = cmd->viewport;
                    std::optional<AttachmentDescription> colorAttachmentDescription = cmd->colorAttachment;
                    std::optional<AttachmentDescription> depthAttachmentDescription = cmd->depthAttachment;
                    VkFormat colorFormat;
                    VkFormat depthFormat;
                    uint32_t colorCount = 0;

                    if (colorAttachmentDescription.has_value()) {
                        AttachmentDescription attachmentDescription = colorAttachmentDescription.value();
                        colorFormat = MapVkFormat(attachmentDescription.format);
                        colorCount = 1;
                    }

                    VkImageView depthImageView;

                    if (depthAttachmentDescription.has_value()) {
                        AttachmentDescription attachmentDescription = depthAttachmentDescription.value();

                        depthImageView = this->depthTexture.ImageView;

                        std::optional<TextureHandle> renderTargetOpt = attachmentDescription.renderTarget;

                        if (renderTargetOpt.has_value()) {
                            TextureHandle renderTarget = renderTargetOpt.value();
                            const auto &depthTex = textureManager->GetTexture(renderTarget);
                            depthImageView = depthTex.ImageView;
                            depthImage = depthTex.Image;

                            depthFormat = MapVkFormat(attachmentDescription.format);

                            mainCommandBuffer.TransitionImageLayout(depthImage, VK_IMAGE_LAYOUT_UNDEFINED,
                                                                 VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
                                                                 VK_IMAGE_ASPECT_DEPTH_BIT, 1, 1);
                        }
                    }

                    mainCommandBuffer.BeginRenderPass(viewport, colorAttachmentDescription, depthAttachmentDescription, frame.ColorTexture.ImageView, depthImageView);
                    secondaryCommandBuffer.Begin(colorCount, colorFormat, depthFormat);
                    break;
                }
                case GLCommandType::BindPipeline: {
                    const auto *cmd = reinterpret_cast<GLCommand_BindPipeline *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_BindPipeline);

                    VulkanPipeline vkPipeline = pipelineManager->GetPipeline(cmd->pipeline);

                    secondaryCommandBuffer.BindPipeline(vkPipeline);
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
                    const VulkanDynamicBuffer &vulkanVertexBuffer = dynamicBuffers[bufferHandle.Id];
                    for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                        if (currentFrame == i) continue;

                        const auto srcMemory = static_cast<uint8_t *>(vulkanVertexBuffer.mappedPointers[i]);
                        memcpy(srcMemory + offset, data, size);
                    }
                    break;
                }
                case GLCommandType::BindVertexBuffer: {
                    const auto *cmd = reinterpret_cast<GLCommand_BindBuffer *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_BindBuffer);

                    BufferHandle bufferHandle = cmd->buffer;
                    VulkanBuffer &buffer = buffers[bufferHandle.Id];

                    secondaryCommandBuffer.BindVertexBuffer(buffer);
                    break;
                }
                case GLCommandType::BindIndexBuffer: {
                    const auto *cmd = reinterpret_cast<GLCommand_BindBuffer *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_BindBuffer);

                    BufferHandle bufferHandle = cmd->buffer;
                    VulkanBuffer &buffer = buffers[bufferHandle.Id];

                    secondaryCommandBuffer.BindIndexBuffer(buffer);
                    break;
                }
                case GLCommandType::BindResourceSet: {
                    const auto *cmd = reinterpret_cast<GLCommand_BindResourceSet *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_BindResourceSet);

                    auto resourceSet = descriptorSets[cmd->resourceId.id];
                    const std::vector<VkDescriptorSet> descriptorSets = resourceSet.set;
                    const auto imageIndex = swapChain->GetCurrentImageIndex();
                    const uint32_t offset = cmd->dynamicOffset;

                    VkPipelineLayout layout;

                    if (cmd->isPipelineLayout) {
                        layout = pipelineManager->GetPipelineLayout(cmd->pipelineLayout);
                    } else {
                        layout = pipelineManager->GetPipeline(cmd->pipeline).pipelineLayout;
                    }

                    secondaryCommandBuffer.BindDescriptorSets(descriptorSets[imageIndex], layout, cmd->index, &offset);
                    break;
                }
                case GLCommandType::Draw: {
                    const auto *cmd = reinterpret_cast<GLCommand_Draw *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_Draw);

                    secondaryCommandBuffer.Draw(cmd->vertexCount, 1, 0, 0);
                    break;
                }
                case GLCommandType::DrawIndexed: {
                    const auto *cmd = reinterpret_cast<GLCommand_DrawIndexed *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_DrawIndexed);

                    uint32_t indexCount = cmd->indexCount;
                    uint32_t indexMesh = cmd->indexMesh;

                    secondaryCommandBuffer.DrawIndexed(indexCount, 1, 0, 0, indexMesh);
                    break;
                }
                case GLCommandType::EndRenderPass: {
                    const auto *cmd = reinterpret_cast<GLCommand_EndRenderPass *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_EndRenderPass);

                    if (depthImage != nullptr) {
                        mainCommandBuffer.TransitionImageLayout(depthImage, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
                                                             VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                             VK_IMAGE_ASPECT_DEPTH_BIT, 1, 1);
                        depthImage = nullptr;
                    } else {
                        mainCommandBuffer.TransitionImageLayout(frame.ColorTexture.Image,
                                                             VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                                             VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_ASPECT_COLOR_BIT,
                                                             1, 1);
                    }

                    secondaryCommandBuffer.End();

                    mainCommandBuffer.ExecuteCommands(secondaryCommandBuffer);

                    break;
                }
                case GLCommandType::UploadBufferData: {
                    const auto *cmd = reinterpret_cast<GLCommand_UploadBufferData *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_UploadBufferData) + cmd->size;

                    auto bufferHandle = cmd->buffer;
                    // auto data = cmd->data;
                    auto data = reinterpret_cast<const uint8_t*>(cmd) + sizeof(GLCommand_UploadBufferData);;
                    auto size = cmd->size;
                    auto offset = cmd->offset;

                    VulkanBuffer& buffer = buffers[bufferHandle.Id];

                    VulkanBuffer staging = bufferManager->CreateHostVisibleBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

                    void* mapped;
                    vkMapMemory(logicalDevice->GetDevice(), staging.memory, 0, size, 0, &mapped);
                    std::memcpy(mapped, data, size);
                    vkUnmapMemory(logicalDevice->GetDevice(), staging.memory);

                    mainCommandBuffer.CopyBuffer(staging.buffer, buffer.buffer, size);
                }
                default: ;
            }
        }

        mainCommandBuffer.End();

        switch (commandBuffer->GetType()) {
            case CommandBufferType::Graphics: {
                commandBufferManager->SendCommandBuffer(mainCommandBuffer, CommandPoolType::Graphics, frame.ImageAvailableSemaphoreVk, frame.RenderFinishedSemaphoreVk, frame.InFlightVk);
                break;
            }
            case CommandBufferType::Compute:
                break;
            case CommandBufferType::Transfer: {
                commandBufferManager->SendCommandBufferAndWait(mainCommandBuffer, CommandPoolType::Transfer);
                break;
            }
        }
    }

    void VulkanRenderDevice::Present() {
        const VulkanFrame &frame = frameManager->GetCurrentFrame();
        swapChain->Present(frame.RenderFinishedSemaphoreVk, frame.imageIndex);
        vkResetCommandBuffer(mainCommandBuffer.GetCommandBuffer(), 0);
    }

    TextureFormat VulkanRenderDevice::GetDisplayFormat() {
        return MapVkFormat(swapChain->GetImageFormat());
    }
} // Rendering
