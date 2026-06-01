//
// Created by zykov on 5/1/2026.
//

#ifndef MAXENGINE_IRENDERDEVICE_H
#define MAXENGINE_IRENDERDEVICE_H
#include "DrawCall.h"
#include "../graphics/Mesh.h"
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

        virtual TextureId CreateTexture(const TextureCreateRequest &desc, const void *initialData) = 0;

        virtual void UpdateTextureFilterMode(TextureId& id, TextureWrap textureWrap) = 0;

        virtual IResourceSet* CreateResourceSet() = 0;

        virtual BufferId CreateBuffer(const BufferDesc &desc, const void *data) = 0;

        virtual void UpdateBuffer(BufferId id, void *data, int offset, int size) = 0;

        virtual void DestroyBuffer(BufferId id) = 0;

        virtual void Prepare() = 0;

        virtual void DrawIndexed(size_t indexCount) = 0;

        virtual void BindVertexBuffer(BufferId id) = 0;

        virtual void BindIndexBuffer(BufferId id) = 0;

        virtual void BindUniformBuffer(BufferId id, uint32_t index) = 0;

        virtual void BindResourceSet(const ResourceSet& resourceSet) = 0;

        virtual void BindPipeline(ShaderId shaderId) = 0;

        virtual void BindMesh(MeshId meshId) = 0;

        virtual void PushConstants(std::string key, Matrix4x4 mat) = 0;

        virtual void PushConstants(const std::string &key, int value) = 0;

        virtual void PushConstants(const std::string &key, float value) = 0;

        virtual void PushConstants(const std::string &key, Color value) = 0;

        virtual void PushConstants(const std::string &key, Vector3 value) = 0;

        virtual void DrawIndexed(unsigned count, MeshId vao) = 0;

        virtual void SetViewport(Viewport viewport) = 0;

        virtual void SetCullMode(CullMode mode) = 0;

        virtual void BeginRenderPass(ClearFlags flags, Color color, float depth) = 0;

        virtual PipelineHandle CreatePipeline(const std::vector<ShaderInput> &shaders, const RenderState &renderStat) = 0;

        virtual PipelineHandle CreatePipelineState(const PipelineStateDesc& pipeline) = 0;

        virtual void SetPipelineState(PipelineHandle pipeline) = 0;

        virtual TextureData GetTextureData(TextureId id) = 0;

        virtual void SubmitCommandBuffer(ICommandBuffer* commandBuffer) = 0;

        virtual void Present() = 0;

    };
} // namespace Rendering


#endif //MAXENGINE_IRENDERDEVICE_H
