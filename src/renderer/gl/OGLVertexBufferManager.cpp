//
// Created by zykov on 7/19/2026.
//

#include "OGLVertexBufferManager.h"

namespace Rendering {
    OGLVertexBufferManager::~OGLVertexBufferManager() {
    }

    BufferHandle OGLVertexBufferManager::CreateBuffer(const BufferDesc &desc, const void *data) {
        const GLsizeiptr size = desc.size;

        GLuint glId;
        glCreateBuffers(1, &glId);

        GLbitfield flags = 0;

        if (desc.isDynamic) {
            flags |= GL_DYNAMIC_STORAGE_BIT;
        }

        glNamedBufferStorage(
            glId,
            static_cast<GLsizeiptr>(size),
            data,
            flags
        );

        const auto id = BufferHandle{.Id = static_cast<uint32_t>(buffers.size())};

        OGLBuffer buffer;
        buffer.glId = glId;

        buffers.push_back(buffer);
        return id;
    }

    BufferHandle OGLVertexBufferManager::CreateVertexBuffer(std::vector<Vertex> vertexes) {
        const uint32_t size = vertexes.size() * sizeof(Vertex);
        const void *data = vertexes.data();

        GLuint glId;
        glCreateBuffers(1, &glId);

        constexpr GLbitfield flags = 0;

        glNamedBufferStorage(
            glId,
            static_cast<GLsizeiptr>(size),
            data,
            flags
        );

        const auto id = BufferHandle{.Id = static_cast<uint32_t>(buffers.size())};

        OGLBuffer buffer;
        buffer.glId = glId;

        buffers.push_back(buffer);
        return id;
    }

    BufferHandle OGLVertexBufferManager::CreateIndexBuffer(std::vector<uint16_t> indices) {
        const uint32_t size = indices.size() * sizeof(uint16_t);
        const void *data = indices.data();

        GLuint glId;
        glCreateBuffers(1, &glId);

        GLbitfield flags = 0;

        glNamedBufferStorage(
            glId,
            static_cast<GLsizeiptr>(size),
            data,
            flags
        );

        const auto id = BufferHandle{.Id = static_cast<uint32_t>(buffers.size())};

        OGLBuffer buffer;
        buffer.glId = glId;

        buffers.push_back(buffer);
        return id;
    }

    BufferHandle OGLVertexBufferManager::CreateUniformBuffer(void *data, const size_t size) {
        GLuint glId;
        glCreateBuffers(1, &glId);

        glNamedBufferStorage(
            glId,
            static_cast<GLsizeiptr>(size),
            data,
            GL_DYNAMIC_STORAGE_BIT
        );

        const auto id = BufferHandle{.Id = static_cast<uint32_t>(buffers.size())};

        OGLBuffer buffer;
        buffer.glId = glId;

        buffers.push_back(buffer);
        return id;
    }

    void OGLVertexBufferManager::UpdateUniformBuffer(const BufferHandle handle, const void *data, const size_t offset, const size_t size) {
        OGLBuffer buffer = buffers[handle.Id];

        glNamedBufferSubData(buffer.glId, static_cast<GLintptr>(offset),
                             static_cast<GLsizeiptr>(size), data);
    }

    OGLBuffer OGLVertexBufferManager::GetBuffer(const BufferHandle handle) const {
        return buffers[handle.Id];
    }
} // Rendering
