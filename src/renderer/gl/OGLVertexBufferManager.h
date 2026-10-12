//
// Created by zykov on 7/19/2026.
//

#ifndef MAXENGINE_OGLVERTEXBUFFERMANAGER_H
#define MAXENGINE_OGLVERTEXBUFFERMANAGER_H

namespace Rendering {
    struct BufferDesc;

    struct OGLBuffer {
        GLuint glId = 0;

        OGLBuffer() = default;

        OGLBuffer(const OGLBuffer&) = delete;
        OGLBuffer& operator=(const OGLBuffer&) = delete;

        OGLBuffer(OGLBuffer&& other) noexcept : glId(other.glId) {
            other.glId = 0;
        }

        OGLBuffer& operator=(OGLBuffer&& other) noexcept {
            if (this != &other) {
                if (glId) glDeleteBuffers(1, &glId);
                glId = other.glId;
                other.glId = 0;
            }
            return *this;
        }

        ~OGLBuffer() {
            if (glId != 0) {
                glDeleteBuffers(1, &glId);
            }
        }
    };

    class OGLVertexBufferManager {
    public:
        ~OGLVertexBufferManager();

        BufferHandle CreateBuffer(const BufferDesc &desc);

        void UpdateUniformBuffer(BufferHandle handle, const void *data, size_t offset, size_t size);

        OGLBuffer& GetBuffer(BufferHandle handle);

    private:
        std::vector<OGLBuffer> buffers;
    };
} // Rendering

#endif //MAXENGINE_OGLVERTEXBUFFERMANAGER_H
