//
// Created by zykov on 7/8/2026.
//

#ifndef MAXENGINE_IVERTEXBUFFERMANAGER_H
#define MAXENGINE_IVERTEXBUFFERMANAGER_H
#include <vector>
#include "../../math/Vertex.h"
#include "buffer/BufferDesc.h"

namespace Rendering {
    struct BufferHandle {
        uint32_t Id;
    };

    class IBufferManager {
    public:
        virtual ~IBufferManager() = default;

        virtual BufferHandle CreateBuffer(const BufferDesc& desc, const void* data) = 0;

        virtual BufferHandle CreateVertexBuffer(std::vector<Vertex> vertexes) = 0;

        virtual BufferHandle CreateIndexBuffer(std::vector<uint16_t> indices) = 0;

        virtual BufferHandle CreateUniformBuffer(void* data, size_t size) = 0;

        virtual void UpdateUniformBuffer(BufferHandle handle, const void* data, size_t offset, size_t size) = 0;
    };
} // Rendering

#endif //MAXENGINE_IVERTEXBUFFERMANAGER_H
