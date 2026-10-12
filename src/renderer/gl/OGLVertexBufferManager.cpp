//
// Created by zykov on 7/19/2026.
//

#include "OGLVertexBufferManager.h"

namespace Rendering {
    OGLVertexBufferManager::~OGLVertexBufferManager() {
    }

    BufferHandle OGLVertexBufferManager::CreateBuffer(const BufferDesc &desc) {
        const GLsizeiptr size = desc.size;

        GLuint glId;
        glCreateBuffers(1, &glId);

        constexpr GLbitfield flags = GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT;

        glNamedBufferStorage(
            glId,
            size,
            nullptr,
            flags
        );

        const auto id = BufferHandle{.Id = static_cast<uint32_t>(buffers.size())};

        OGLBuffer buffer;
        buffer.glId = glId;

        buffers.push_back(std::move(buffer));
        return id;
    }

    void OGLVertexBufferManager::UpdateUniformBuffer(const BufferHandle handle, const void *data, const size_t offset, const size_t size) {
        const OGLBuffer& buffer = GetBuffer(handle);

        glNamedBufferSubData(buffer.glId, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size), data);
    }

    OGLBuffer& OGLVertexBufferManager::GetBuffer(const BufferHandle handle) {
        return buffers[handle.Id];
    }
} // Rendering
