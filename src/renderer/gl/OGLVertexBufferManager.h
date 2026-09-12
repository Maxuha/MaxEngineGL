//
// Created by zykov on 7/19/2026.
//

#ifndef MAXENGINE_OGLVERTEXBUFFERMANAGER_H
#define MAXENGINE_OGLVERTEXBUFFERMANAGER_H
#include "../domain/IBufferManager.h"

namespace Rendering {
    struct OGLBuffer {
        GLuint glId;
    };

    class OGLVertexBufferManager : public IBufferManager {
    public:
        ~OGLVertexBufferManager() override;

        BufferHandle CreateBuffer(const BufferDesc &desc, const void *data) override;

        BufferHandle CreateVertexBuffer(std::vector<Vertex> vertexes) override;

        BufferHandle CreateIndexBuffer(std::vector<uint16_t> indices) override;

        BufferHandle CreateUniformBuffer(void *data, size_t size) override;

        void UpdateUniformBuffer(BufferHandle handle, const void *data, size_t offset, size_t size) override;

        OGLBuffer GetBuffer(BufferHandle handle) const;

    private:
        std::vector<OGLBuffer> buffers;
    };
} // Rendering

#endif //MAXENGINE_OGLVERTEXBUFFERMANAGER_H
