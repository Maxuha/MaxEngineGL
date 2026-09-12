//
// Created by zykov on 5/22/2026.
//

#ifndef MAXENGINE_GLCOMMANDBUFFER_H
#define MAXENGINE_GLCOMMANDBUFFER_H
#include <stdexcept>
#include <vector>

#include "../ICommandBufferManager.h"
#include "../ICommandPoolManager.h"
#include "../IFrameBufferManager.h"
#include "../IRenderPassManager.h"
#include "../IPipelineManager.h"
#include "../../math/Color.h"
#include "../../math/Rect.h"
#include "../../math/Matrix4x4.h"
#include "../domain/Material.h"
#include "../domain/struct/PipelineStateDesc.h"
#include "../IResourceSetManager.h"


namespace Rendering {
    class OGLFrameBufferManager;
    class OGLRenderPassManager;
    class GLPipeline;
    struct RenderPass;
    struct AttachmentDescription;
    struct RenderPassHandle;
    struct FrameBufferHandle;
    struct PipelineHandle;
    struct TextureHandle;
    struct CommandBufferHandle;

    enum class GLCommandType : uint8_t {
        BeginRenderPass,
        EndRenderPass,
        BindPipeline,
        BindMaterial,
        BindVertexBuffer,
        BindIndexBuffer,
        BindResourceSet,
        PushConstants,
        UpdateBuffer,
        Draw,
        DrawIndexed,
        UpdateUniformBuffer
    };

    struct GLCommand_BeginRenderPass {
        AttachmentDescription attachment1{};
        AttachmentDescription attachment2{};
        GLuint fboId{};
        uint32_t width{}, height{};
        Color clearColor;

        RenderPassHandle renderPass{};
        FrameBufferHandle frameBuffer{};
        Rect viewport{};
    };

    struct GLCommand_EndRenderPass {
        AttachmentDescription attachment{};
    };

    struct GLCommand_BindPipeline {
        PipelineHandle pipeline;
    };

    struct GLCommand_Draw {
        uint32_t vertexCount;
    };

    struct GLCommand_DrawIndexed {
        uint32_t indexCount;
    };

    struct GLCommand_BindBuffer {
          BufferHandle buffer;
    };

    struct GLCommand_BindMaterial {
        MaterialHandle material;
    };

    struct GLCommand_BindResourceSet {
        ResourceSetHandle resourceId;
        PipelineHandle pipeline;
        uint32_t index;
    };

    struct GLCommand_UpdateBuffer {
        BufferHandle buffer;
        size_t offset;
        size_t size;
        const void *data;
    };

    struct GLCommand_PushConstants {
        uint32_t key{};
        Matrix4x4 mat;
        uint32_t value{};
    };

    struct GLCommand_UpdateUniformBuffer {
        BufferHandle buffer;
        size_t offset;
        size_t size;
        const void *data;
    };

    class CommandBuffer {
    public:
        explicit CommandBuffer(size_t bufferSize = 1024 * 1024);
        ~CommandBuffer();

        void Begin();

        void End();

        void Draw(uint32_t vertexCount);

        void DrawIndexed(uint32_t indexCount);

        void BeginRenderPass(const AttachmentDescription &attachment, GLuint fboId, ::Color clearColor);

        void BeginRenderPass(const AttachmentDescription &attachment, GLuint fboId, int w, int h,
            ::Color clearColor);

        void BeginRenderPass(RenderPassHandle renderPass, FrameBufferHandle frameBuffer, const ::Rect &viewport,
            ::Color clearColor);

        void BeginRenderPass(RenderPassHandle renderPass, Color clearColor);

        void BeginRenderPass(const RenderPass &renderPass);

        void EndRenderPass();

        void EndRenderPass(const AttachmentDescription& attachment);

        void BindPipeline(const PipelineHandle& pipeline);

        void BindMaterial(const MaterialHandle& material);

        void ResetAll();

        void BindVertexBuffer(BufferHandle& buffer);

        void BindIndexBuffer(BufferHandle& buffer);

        void UpdateBuffer(BufferHandle id, const void *data, int offset, int size);

        void BindResourceSet(ResourceSetHandle setIndex, PipelineHandle pipelineHandle, uint32_t index);

        void PushConstants(uint32_t key, Matrix4x4 mat);

        void PushConstants(const std::string &key, uint32_t value);

        void UpdateUniformBuffer(BufferHandle buffer, const void *data, int offset, int size);

        std::vector<uint8_t>& GetCommands();

        size_t GetHead() const;

        CommandBufferHandle GetHandle() const;

        CommandPoolHandle GetPool() const;

        PipelineHandle GetPipeline() const;

        void Reset();

    private:
        std::vector<uint8_t> buffer;
        size_t head = 0;
        PipelineHandle pipeline{};

        template<typename T>
        T *allocateCommand(const GLCommandType type) {
            static_assert(std::is_trivially_copyable_v<T>, "The type should be Trivially Copyable!");
            size_t sizeNeeded = sizeof(GLCommandType) + sizeof(T);
            if (head + sizeNeeded > buffer.size()) {
                throw std::runtime_error("GLCommandBuffer layout overflow!");
            }

            *reinterpret_cast<GLCommandType *>(&buffer[head]) = type;
            head += sizeof(GLCommandType);

            T *cmdData = reinterpret_cast<T *>(&buffer[head]);
            head += sizeof(T);

            return cmdData;
        }
    };
}

#endif //MAXENGINE_GLCOMMANDBUFFER_H
