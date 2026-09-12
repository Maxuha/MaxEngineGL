//
// Created by zykov on 6/24/2026.
//

#include "VulkanFrameManager.h"

namespace Rendering {
    VulkanFrameManager::~VulkanFrameManager() {
    }

    VulkanFrameManager::VulkanFrameManager(ILogicalDevice &device, VulkanSwapChain &swapchain,
                                           IFrameBufferManager &frameBufferManager,
                                           ICommandBufferManager &commandBufferManager)
        : device(static_cast<VulkanLogicalDevice *>(&device)),
          swapchain(&swapchain),
          frameBufferManager(&frameBufferManager),
          commandBufferManager(&commandBufferManager) {

        frames.reserve(3);

        CreateFrames(RenderPassHandle{}, TextureHandle{.Id = 0});
        CreateFrames(RenderPassHandle{}, TextureHandle{.Id = 0});
        CreateFrames(RenderPassHandle{}, TextureHandle{.Id = 0});
    }

    void VulkanFrameManager::CreateFrames(const RenderPassHandle renderPass, const TextureHandle texture) {
        const VkCommandBuffer commandBuffer = static_cast<VulkanCommandBufferManager*>(commandBufferManager)->AllocateCommandBuffer(CommandPoolType::Graphics, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        const VkCommandBuffer secondaryCommandBuffer = static_cast<VulkanCommandBufferManager*>(commandBufferManager)->AllocateCommandBuffer(CommandPoolType::Graphics, VK_COMMAND_BUFFER_LEVEL_SECONDARY);

        VulkanFrame frame;
        frame.ImageAvailableSemaphoreVk = device->CreateSemaphor();
        frame.RenderFinishedSemaphoreVk = device->CreateSemaphor();
        frame.InFlightVk = device->CreateFence();
        frame.commandBuffer = commandBuffer;
        frame.secondaryCommandBuffer = secondaryCommandBuffer;

        frames.push_back(frame);
    }

    VulkanFrame& VulkanFrameManager::AcquireFrame() {
        constexpr int MAX_FRAMES_IN_FLIGHT = 3;

        VulkanFrame& frame = frames[currentFrame];

        const Result result = device->WaitIdle(frame.InFlightVk);

        uint32_t index = 0;
        swapchain->AcquireNextImage(new VulkanSemaphore(frame.ImageAvailableSemaphoreVk), index);

        if (!result.Success) {
            throw std::runtime_error("failed to wait for idle!" + result.Message);
        }

        frame.ColorTexture = swapchain->GetSwapChainImageView(index);
        frame.DepthImageView = swapchain->GetDepthImageView();
        frame.imageIndex = index;

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

        return frame;
    }

    void VulkanFrameManager::SendFrameToGPU(const VulkanFrame &frame) const {
       // const VkCommandBuffer cmdBuffer = static_cast<VulkanCommandBufferManager *>(commandBufferManager)->AllocateCommandBuffer(CommandPoolType::Graphics, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        const VkSemaphore waitSemaphores[] = {frame.ImageAvailableSemaphoreVk};
        constexpr VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &frame.commandBuffer;

        const VkSemaphore signalSemaphores[] = {frame.RenderFinishedSemaphoreVk};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (const VkResult result = vkQueueSubmit(device->GetGraphicsQueue(), 1, &submitInfo, frame.InFlightVk);
            result != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!: " + std::to_string(result));
        }
    }

    void VulkanFrameManager::Present(const VulkanFrame &frame) const {
        swapchain->Present(&frame);
    }

    CommandBufferHandle VulkanFrameManager::GetCommandBufferForCurrentFrame() const {
       // return frames[currentFrame].commandBuffer;
        return CommandBufferHandle {};
    }

    FrameBufferHandle VulkanFrameManager::GetCurrentFrame() const {
        return frames[currentFrame].FrameBuffer;
    }

    VulkanFrame VulkanFrameManager::GetVulkanFrame() const {
        return frames[currentFrame];
    }

    uint32_t VulkanFrameManager::GetCurrentIndexFrame() const {
        return currentFrame;
    }
} // Rendering
