//
// Created by zykov on 5/22/2026.
//

#ifndef MAXENGINE_GLCOMMANDBUFFER_H
#define MAXENGINE_GLCOMMANDBUFFER_H
#include <stdexcept>
#include <vector>

#include "../IPipelineManager.h"
#include "../../math/Rect.h"
#include "../../math/Matrix4x4.h"
#include "../domain/buffer/BufferHandle.h"
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
        Draw,
        DrawIndexed,
        UpdateBuffer,
        UploadBufferData
    };

    struct GLCommand_BeginRenderPass {
        std::optional<AttachmentDescription> colorAttachment{};
        std::optional<AttachmentDescription> depthAttachment{};
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
        uint32_t indexMesh;
        uint32_t firstIndex;
        int32_t vertexOffset;
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
        PipelineLayoutHandle pipelineLayout;
        bool isPipelineLayout;
        uint32_t index;
        uint32_t dynamicOffset;
    };

    struct GLCommand_PushConstants {
        uint32_t key{};
        Matrix4x4 mat;
        uint32_t value{};
    };

    struct GLCommand_UpdateBuffer {
        BufferHandle buffer;
        size_t offset;
        size_t size;
        const void *data;
    };

    struct GLCommand_UploadBufferData {
        BufferHandle buffer;
        size_t offset;
        size_t size;
        const void *data;
    };

    enum class CommandBufferType {
        Graphics,
        Compute,
        Transfer
    };

    class CommandBuffer {
    public:
        explicit CommandBuffer(size_t bufferSize = 1024 * 1024, CommandBufferType type = CommandBufferType::Graphics);
        ~CommandBuffer();

        void Begin();

        void End();

        void Draw(uint32_t vertexCount);

        void DrawIndexed(uint32_t indexCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t indexMesh);

        void BeginRenderPass(const RenderPass &renderPass);

        void EndRenderPass();

        void EndRenderPass(const AttachmentDescription& attachment);

        void BindPipeline(const PipelineHandle& pipeline);

        void BindMaterial(const MaterialHandle& material);

        void BindVertexBuffer(const BufferHandle& buffer);

        void BindIndexBuffer(const BufferHandle& buffer);

        void BindResourceSet(ResourceSetHandle setIndex, PipelineHandle pipelineHandle, uint32_t index, uint32_t offset = 0);

        void BindResourceSet(ResourceSetHandle setIndex, PipelineLayoutHandle pipelineLayoutHandle, uint32_t index, uint32_t offset = 0);

        void PushConstants(uint32_t key, Matrix4x4 mat);

        void PushConstants(const std::string &key, uint32_t value);

        void UpdateBuffer(BufferHandle buffer, const void *data, int offset, int size);

        void UploadBufferData(BufferHandle buffer, const void *data, int offset, int size);

        std::vector<uint8_t>& GetCommands();

        size_t GetHead() const;

        PipelineHandle GetPipeline() const;

        CommandBufferType GetType() const;

        void Reset();

    private:
        std::vector<uint8_t> buffer;
        size_t head = 0;
        PipelineHandle pipeline{};

        CommandBufferType type;

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
