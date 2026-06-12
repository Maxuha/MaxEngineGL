//
// Created by zykov on 5/22/2026.
//

#ifndef MAXENGINE_GLCOMMANDBUFFER_H
#define MAXENGINE_GLCOMMANDBUFFER_H
#include <stdexcept>
#include <vector>
#include "../domain/GLResourceSet.h"
#include "../domain/ICommandBuffer.h"
#include "../IRenderDevice.h"
#include "../../../cmake-build-debug/_deps/glfw-src/src/internal.h"
#include "../domain/struct/Buffer.h"
#include "../../math/Color.h"

namespace Rendering {
    class GLPipeline;
    class Color;

    enum class GLCommandType : uint8_t {
        BeginRenderPass,
        EndRenderPass,
        BindPipeline,
        BindVertexBuffer,
        BindIndexBuffer,
        BindUniformBuffer,
        BindResourceSet,
        PushConstants,
        UpdateBuffer,
        Draw,
    };

    struct GLCommand_BeginRenderPass {
        AttachmentDescription attachment{};
        GLuint fboId{};
        int width{}, height{};
        ::Color clearColor;
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

    struct GLCommand_BindBuffer {
          BufferHandle buffer;
    };

    struct GLCommand_BindUniform {
        uint32_t bindingPoint;
        BufferHandle buffer;
    };

    struct GLCommand_BindResourceSet {
          const GLResourceSet* resourceSet;
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

    class GLCommandBuffer : public ICommandBuffer {
    public:
        explicit GLCommandBuffer(size_t bufferSize = 1024 * 1024);
        ~GLCommandBuffer() override;

        void Begin() override;

        void End() override;

        void Draw(uint32_t vertexCount) override;

        void BeginRenderPass(const AttachmentDescription& attachment, GLuint fboId, int w, int h, ::Color clearColor) override;

        void BeginRenderPass(const AttachmentDescription &attachment, GLuint fboId, ::Color clearColor) override;

        void EndRenderPass(const AttachmentDescription& attachment) override;

        void BindPipeline(const PipelineHandle& pipeline) override;

        void ResetAll() override;

        void BindVertexBuffer(BufferHandle& buffer) override;

        void BindIndexBuffer(BufferHandle& buffer) override;

        void BindUniformBuffer(uint32_t bindingPoint, const BufferHandle& buffer) override;

        void UpdateBuffer(BufferHandle id, void *data, int offset, int size) override;

        void BindResourceSet(uint32_t setIndex, const IResourceSet &resourceSet) override;

        void PushConstants(uint32_t key, Matrix4x4 mat) override;

        void PushConstants(const std::string &key, uint32_t value) override;

        std::vector<uint8_t>& GetCommands() override;

        size_t GetHead() const override;

        PipelineHandle GetPipeline() const override;

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
