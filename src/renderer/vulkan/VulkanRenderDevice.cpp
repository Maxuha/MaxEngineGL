//
// Created by zykov on 6/16/2026.
//

#include "VulkanRenderDevice.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <map>
#include <vulkan/vulkan_core.h>

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

        const TextureHandle texture = textureManager->CreateTexture(rect.width, rect.height, TextureFormat::DEPTH,
                                                                    nullptr);
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

    ResourceSetHandle VulkanRenderDevice::CreateResourceSet(const PipelineHandle pipelineHandle,
                                                            const ResourceBinding resource) {
        std::vector<VkDescriptorSet> descriptorSets;

        const VulkanPipeline pipeline = pipelineManager->GetPipeline(pipelineHandle);

        if (resource.resourceId == 999999) {
            descriptorSets = descriptorSetManager->CreateDescriptorSets(pipeline.descriptionSetLayouts[0],
                                                                        MAX_FRAMES_IN_FLIGHT);

            VulkanDescriptorSet vulkanDescriptorSet;
            vulkanDescriptorSet.set = descriptorSets;

            this->descriptorSets.push_back(vulkanDescriptorSet);

            return ResourceSetHandle{.id = this->descriptorSets.size() - 1};
        }
        if (resource.resourceId == 999998) {
            descriptorSets = descriptorSetManager->CreateDescriptorSets(pipeline.descriptionSetLayouts[1],
                                                                        MAX_FRAMES_IN_FLIGHT);

            VulkanDescriptorSet vulkanDescriptorSet;
            vulkanDescriptorSet.set = descriptorSets;

            this->descriptorSets.push_back(vulkanDescriptorSet);

            return ResourceSetHandle{.id = this->descriptorSets.size() - 1};
        }

        descriptorSets = descriptorSetManager->CreateDescriptorSets(pipeline.descriptionSetLayouts[2],
                                                                    MAX_FRAMES_IN_FLIGHT);

        VulkanDescriptorSet vulkanDescriptorSet;
        vulkanDescriptorSet.set = descriptorSets;

        this->descriptorSets.push_back(vulkanDescriptorSet);

        return ResourceSetHandle{.id = this->descriptorSets.size() - 1};
    }

    ResourceSetHandle VulkanRenderDevice::CreateResourceSet(const ResourceSetLayoutHandle resourceSetLayoutHandle) {
        const VkDescriptorSetLayout descriptorSetLayout = descriptorSetManager->GetDescriptorSetLayout(resourceSetLayoutHandle);
        const std::vector<VkDescriptorSet> descriptorSets = descriptorSetManager->CreateDescriptorSets(descriptorSetLayout, MAX_FRAMES_IN_FLIGHT);
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
        constexpr int MAX_FRAMES_IN_FLIGHT = 3;

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

    BufferHandle VulkanRenderDevice::CreateFrameBuffer(TextureHandle texture) {
        return {};
    }

    void VulkanRenderDevice::UpdateBuffer(const BufferHandle id, const void *data, const int offset, const int size) {
        VulkanVertexBuffer &buffer = buffers[id.Id];
        bufferManager->UpdateInternalUniformBuffer(buffer, data, offset, size);
    }

    void VulkanRenderDevice::DestroyBuffer(BufferHandle id) {
    }

    void VulkanRenderDevice::BindUniformBuffer(BufferHandle id, uint32_t index) {
    }

    PipelineHandle VulkanRenderDevice::CreatePipeline(const std::vector<ShaderHandle> &shaders,
                                                      const RenderPassHandle renderPass,
                                                      const PipelineStateDesc &pipelineStateDesc) {
        return pipelineManager->CreatePipeline(shaders, renderPass, pipelineStateDesc);
    }

    PipelineHandle VulkanRenderDevice::CreatePipeline(const ShaderHandle shader,
                                                      const PipelineStateDesc &pipelineStateDesc) {
        const GraphicsVulkanShader vulkanShader = shaders[shader.Id];
        return pipelineManager->CreatePipeline(vulkanShader, pipelineStateDesc);
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

        for (auto resource_set_layout : shaderDesc.resourceSetLayouts) {
            ResourceSetLayoutHandle resource_set_layout_handle = CreateResourceSetLayout(resource_set_layout);
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
        const VkCommandBuffer vkSecondaryCommandBuffer = commandBufferManager->AllocateCommandBuffer(
            CommandPoolType::Graphics, VK_COMMAND_BUFFER_LEVEL_SECONDARY);

        frame.commandBuffer = vkCommandBuffer;
        frame.secondaryCommandBuffer = vkSecondaryCommandBuffer;

        vkResetCommandBuffer(vkCommandBuffer, 0);
        vkResetCommandBuffer(vkSecondaryCommandBuffer, 0);

        VkFormat colorFormat = VK_FORMAT_B8G8R8A8_SRGB;

        VkCommandBufferInheritanceRenderingInfo inheritanceRenderingInfo{};
        inheritanceRenderingInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_RENDERING_INFO;
        inheritanceRenderingInfo.colorAttachmentCount = 1;
        inheritanceRenderingInfo.pColorAttachmentFormats = &colorFormat;
        inheritanceRenderingInfo.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;
        inheritanceRenderingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkCommandBufferInheritanceInfo inheritanceInfo{};
        inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
        inheritanceInfo.pNext = &inheritanceRenderingInfo;

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;

        VkCommandBufferBeginInfo secondaryBeginInfo{};
        secondaryBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        secondaryBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
        secondaryBeginInfo.pInheritanceInfo = &inheritanceInfo;

        vkBeginCommandBuffer(vkCommandBuffer, &beginInfo);

        std::vector<uint8_t> commands = commandBuffer->GetCommands();
        size_t readOffset = 0;
        const size_t head = commandBuffer->GetHead();

        VulkanPipeline currentPipeline;

        while (readOffset < head) {
            const GLCommandType type = *reinterpret_cast<GLCommandType *>(&commands[readOffset]);
            readOffset += sizeof(GLCommandType);
            switch (type) {
                case GLCommandType::BeginRenderPass: {
                    const auto *cmd = reinterpret_cast<GLCommand_BeginRenderPass *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_BeginRenderPass);

                    VkImageMemoryBarrier2 colorBarrier{};
                    colorBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
                    colorBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
                    colorBarrier.srcAccessMask = 0;
                    colorBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
                    colorBarrier.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
                    colorBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                    colorBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    colorBarrier.image = frame.ColorTexture.Image;
                    colorBarrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

                    VkImageMemoryBarrier2 preRenderBarriers[] = {colorBarrier};

                    VkDependencyInfo preRenderDependency{};
                    preRenderDependency.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
                    preRenderDependency.imageMemoryBarrierCount = 1;
                    preRenderDependency.pImageMemoryBarriers = preRenderBarriers;

                    vkCmdPipelineBarrier2(vkCommandBuffer, &preRenderDependency);

                    // --- END

                    Color color = cmd->clearColor;
                    Rect viewport = cmd->viewport;
                    viewport = Rect(viewport.x, viewport.y, 2160, 1440);

                    VkRenderingAttachmentInfo colorAttachment{};
                    colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
                    colorAttachment.imageView = frame.ColorTexture.ImageView; // <-- Render directly to the view
                    colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                    colorAttachment.clearValue.color = {color.r, color.g, color.b, color.a};

                    VkRenderingAttachmentInfo depthAttachment{};
                    depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
                    depthAttachment.imageView = frame.DepthImageView; // <-- Render directly to the view
                    depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                    depthAttachment.clearValue.depthStencil.depth = 1.0f;
                    depthAttachment.clearValue.depthStencil.stencil = 0;

                    VkRenderingInfo renderingInfo{};
                    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
                    renderingInfo.flags = VK_RENDERING_CONTENTS_SECONDARY_COMMAND_BUFFERS_BIT;
                    renderingInfo.renderArea = {
                        {static_cast<int32_t>(viewport.x), static_cast<int32_t>(viewport.y)},
                        {static_cast<uint32_t>(viewport.width), static_cast<uint32_t>(viewport.height)}
                    };
                    renderingInfo.layerCount = 1;
                    renderingInfo.colorAttachmentCount = 1;
                    renderingInfo.pColorAttachments = &colorAttachment; // <-- Pass your attachment info
                    renderingInfo.pDepthAttachment = &depthAttachment;
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
                    currentPipeline = vkPipeline;
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

                    // auto resourceSet = descriptorSets[cmd->resourceId.id];

                    // auto resourceSet = descriptorSetManager->GetDescriptorSet(cmd->resourceId);
                    auto resourceSet = descriptorSets[cmd->resourceId.id];
                    const std::vector<VkDescriptorSet> descriptorSets = resourceSet.set;
                    const auto imageIndex = swapChain->GetCurrentImageIndex();

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

                    VkImageMemoryBarrier2 presentBarrier{};
                    presentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
                    presentBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
                    presentBarrier.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
                    presentBarrier.dstStageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
                    presentBarrier.dstAccessMask = 0;
                    presentBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    presentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                    presentBarrier.image = frame.ColorTexture.Image;
                    presentBarrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

                    VkDependencyInfo postRenderDependency{};
                    postRenderDependency.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
                    postRenderDependency.imageMemoryBarrierCount = 1;
                    postRenderDependency.pImageMemoryBarriers = &presentBarrier;

                    vkCmdPipelineBarrier2(vkCommandBuffer, &postRenderDependency);

                    break;
                }
                default: ;
            }
        }

        vkEndCommandBuffer(vkCommandBuffer);

        frameManager->SendFrameToGPU(frame);

        frameManager->Present(frame);
    }

    void VulkanRenderDevice::Present() {
    }

    ImageFormat VulkanRenderDevice::GetDisplayFormat() {
        return MapVkFormat(swapChain->GetImageFormat());
    }
} // Rendering
