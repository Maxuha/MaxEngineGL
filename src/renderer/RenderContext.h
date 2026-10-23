//
// Created by zykov on 6/8/2026.
//

#ifndef MAXENGINE_RENDERERCONTEXT_H
#define MAXENGINE_RENDERERCONTEXT_H
#include "ICommandBufferManager.h"


namespace Rendering {
    class VulkanDescriptorSetManager;
    class IBufferManager;
    class VulkanFrameManager;
    class ICommandPoolManager;
    class IShaderManager;
    class ITextureManager;
    class IFrameBufferManager;
    class IPipelineManager;
    class ILogicalDevice;
    class IPhysicalDevice;
    class IRenderPassManager;
    class VulkanSwapChain;
    class IResourceSetManager;
    class OGLTextureManager;


    // Mediator for RenderSystem
    struct RenderContext {
        IPhysicalDevice* PhysicalDevice;
        ILogicalDevice* LogicalDevice;
        IRenderPassManager* RenderPassManager;
        IPipelineManager* PipelineManager;
        IFrameBufferManager* FrameBufferManager;
        OGLTextureManager* TextureManager;
        IShaderManager* ShaderManager;
        ICommandPoolManager* CommandPoolManager;
        ICommandBufferManager* CommandBufferManager;
        IBufferManager* BufferManager;
        VulkanFrameManager* FrameManager;
        VulkanSwapChain* Swapchain;
        IResourceSetManager* DescriptorSetManager;
    };
} // Rendering

#endif //MAXENGINE_RENDERERCONTEXT_H
