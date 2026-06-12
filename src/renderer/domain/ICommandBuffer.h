//
// Created by zykov on 5/22/2026.
//

#ifndef MAXENGINE_COMMANDBUFFER_H
#define MAXENGINE_COMMANDBUFFER_H

#include "IPipeline.h"
#include "IResourceSet.h"
#include "glad/glad.h"
#include "glm/fwd.hpp"
#include "../domain/struct/PipelineHandle.h"
#include "../../math/Matrix4x4.h"
#include "../../math/Color.h"
#include <string>

namespace Rendering {
    enum class AttachmentLoadOp { LOAD, CLEAR, DONT_CARE };

    enum class AttachmentStoreOp { STORE, DONT_CARE };

    enum class AttachmentType : uint32_t {
        NONE = 0,
        COLOR = 1 << 0,
        DEPTH = 1 << 1,
        STENCIL = 1 << 2
    };

    inline AttachmentType operator|(AttachmentType a, AttachmentType b) {
        return static_cast<AttachmentType>(
            static_cast<std::underlying_type_t<AttachmentType>>(a) |
            static_cast<std::underlying_type_t<AttachmentType>>(b)
        );
    }

    inline AttachmentType operator&(AttachmentType a, AttachmentType b) {
        return static_cast<AttachmentType>(
            static_cast<std::underlying_type_t<AttachmentType>>(a) &
            static_cast<std::underlying_type_t<AttachmentType>>(b)
        );
    }

    struct AttachmentDescription {
        AttachmentType type;
        AttachmentLoadOp loadOp;
        AttachmentStoreOp storeOp;
    };

    class ICommandBuffer {
    public:
        virtual ~ICommandBuffer() = default;

        virtual void Begin() = 0;

        virtual void End() = 0;

        virtual void BeginRenderPass(const AttachmentDescription& attachment, GLuint fboId, int w, int h,Color clearColor) = 0;

        virtual void BeginRenderPass(const AttachmentDescription& attachment, GLuint fboId, Color clearColor) = 0;

        virtual void EndRenderPass(const AttachmentDescription& attachment) = 0;

        virtual void Draw(glm::uint32_t vertexCount) = 0;

        virtual void BindPipeline(const PipelineHandle &pipeline) = 0;

        virtual void ResetAll() = 0;

        virtual void BindVertexBuffer(BufferHandle &buffer) = 0;

        virtual void BindIndexBuffer(BufferHandle &buffer) = 0;

        virtual void BindUniformBuffer(uint32_t bindingPoint, const BufferHandle &buffer) = 0;

        virtual void UpdateBuffer(BufferHandle id, void *data, int offset, int size) = 0;

        virtual void BindResourceSet(uint32_t setIndex, const IResourceSet &resourceSet) = 0;

        virtual void PushConstants(uint32_t key, Matrix4x4 mat) = 0;

        virtual void PushConstants(const std::string &key, uint32_t value) = 0;

        virtual std::vector<uint8_t>& GetCommands() = 0;

        virtual size_t GetHead() const = 0;

        virtual PipelineHandle GetPipeline() const = 0;
    };
} // Rendering

#endif //MAXENGINE_COMMANDBUFFER_H
