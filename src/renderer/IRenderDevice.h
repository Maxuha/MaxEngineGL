//
// Created by zykov on 5/1/2026.
//

#ifndef MAXENGINE_IRENDERDEVICE_H
#define MAXENGINE_IRENDERDEVICE_H
#include "DrawCall.h"
#include "domain/Mesh.h"
#include "domain/GLPipeline.h"
#include "domain/IResourceSet.h"
#include "domain/struct/Pipeline.h"
#include "domain/struct/PipelineHandle.h"
#include "domain/struct/PipelineStateDesc.h"
#include "domain/struct/ResourceSet.h"
#include "gl/desc/TextureCreateRequest.h"
#include "gl/model/TextureData.h"


namespace Rendering {
    struct Viewport {
        float x, y, width, height;
    };

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

        virtual ShaderId CreateShader(const char *vCode, const char *fCode) = 0;

        virtual TextureHandle CreateTexture(const TextureCreateRequest &desc, const void *initialData) = 0;

        virtual void UpdateTextureFilterMode(TextureHandle& id, TextureWrap textureWrap) = 0;

        virtual IResourceSet* CreateResourceSet() = 0;

        virtual BufferHandle CreateBuffer(const BufferDesc &desc, const void *data) = 0;

        virtual BufferHandle CreateFrameBuffer(TextureHandle texture) = 0;

        virtual void UpdateBuffer(BufferHandle id, void *data, int offset, int size) = 0;

        virtual void DestroyBuffer(BufferHandle id) = 0;

        virtual void BindUniformBuffer(BufferHandle id, uint32_t index) = 0;

        virtual PipelineHandle CreatePipeline(const std::vector<ShaderInput> &shaders, const PipelineStateDesc &pipelineStateDesc) = 0;

        virtual void SubmitCommandBuffer(ICommandBuffer* commandBuffer) = 0;

        virtual void Present() = 0;

    };
} // namespace Rendering


#endif //MAXENGINE_IRENDERDEVICE_H
