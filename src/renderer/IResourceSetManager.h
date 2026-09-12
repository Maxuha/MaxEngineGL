//
// Created by zykov on 8/8/2026.
//

#ifndef MAXENGINE_IRESOURCESETMANAGER_H
#define MAXENGINE_IRESOURCESETMANAGER_H
#include <cstdint>

#include "domain/IBufferManager.h"

namespace Rendering {
    struct TextureHandle;

    struct DescriptorPoolHandle { uint32_t id; };
    struct ResourceSetHandle { uint32_t id; };
    struct ResourceSetLayoutHandle { uint32_t Id; };

    class IResourceSetManager {
    public:
        virtual ~IResourceSetManager() = default;

        virtual void BindBuffer(ResourceSetHandle setHandle, uint32_t slot, BufferHandle bufferHandle) = 0;

        virtual void BindTexture(ResourceSetHandle setHandle, uint32_t slot, TextureHandle textureHandle) = 0;
    };
} // Rendering

#endif //MAXENGINE_IRESOURCESETMANAGER_H
