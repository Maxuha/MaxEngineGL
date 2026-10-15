//
// Created by zykov on 5/1/2026.
//

#ifndef MAXENGINE_IRENDERDEVICE_H
#define MAXENGINE_IRENDERDEVICE_H
#include "IPipelineManager.h"
#include "domain/struct/ResourceSet.h"
#include "gl/model/TextureData.h"
#include "domain/struct/PipelineStateDesc.h"
#include "gl/CommandBuffer.h"


class IWindow;

namespace Rendering {
    class IFrameManager;
    class IPipelineManager;
    class ICommandBufferManager;
    class ICommandPoolManager;
    class IShaderManager;
    class IFrameBufferManager;
    class IRenderPassManager;
    class ITextureManager;

    enum class ClearFlags : uint32_t {
        None = 0,
        Color = 1 << 0,
        Depth = 1 << 1,
        Stencil = 1 << 2
    };

    inline ClearFlags operator|(ClearFlags lhs, ClearFlags rhs) {
        return static_cast<ClearFlags>(
            static_cast<std::underlying_type_t<ClearFlags>>(lhs) |
            static_cast<std::underlying_type_t<ClearFlags>>(rhs)
        );
    }

    inline ClearFlags operator&(ClearFlags lhs, ClearFlags rhs) {
        return static_cast<ClearFlags>(
            static_cast<std::underlying_type_t<ClearFlags>>(lhs) &
            static_cast<std::underlying_type_t<ClearFlags>>(rhs)
        );
    }

    struct BufferDesc;

    class IRenderDevice {
    public:
        virtual ~IRenderDevice() = default;

        virtual CommandBuffer* AllocateCommandBuffer(CommandBufferType type) = 0;

        virtual void CreateVao(VertexLayout layout) = 0;

        virtual TextureHandle CreateTexture(const TextureCreateRequest &desc, const void *initialData) = 0;

        virtual PipelineLayoutHandle CreatePipelineLayout(const std::vector<ResourceSetLayoutHandle>& resourceSetLayouts) = 0;

        virtual ResourceSetLayoutHandle CreateResourceSetLayout(const ResourceSetLayoutDesc& resourceSetLayoutDesc) = 0;

        virtual ResourceSetHandle CreateResourceSet(ResourceSetLayoutHandle resourceSetLayoutHandle) = 0;

        virtual void UpdateResourceSet(ResourceSetHandle setHandle, uint32_t binding, TextureHandle textureHandle) = 0;

        virtual void UpdateResourceSet(ResourceSetHandle setHandle, uint32_t binding, BufferHandle bufferHandle) = 0;

        virtual BufferHandle CreateBuffer(const BufferDesc &desc) = 0;

        virtual void UpdateBuffer(BufferHandle id, const void *data, int offset, int size) = 0;

        virtual void DestroyBuffer(BufferHandle id) = 0;

        virtual PipelineHandle CreatePipeline(const ShaderDesc &shaderDesc, const PipelineStateDesc &pipelineStateDesc) = 0;

        virtual void SubmitCommandBuffer(CommandBuffer* commandBuffer) = 0;

        virtual void Present() = 0;

        virtual TextureFormat GetDisplayFormat() = 0;

    };
} // namespace Rendering


#endif //MAXENGINE_IRENDERDEVICE_H
