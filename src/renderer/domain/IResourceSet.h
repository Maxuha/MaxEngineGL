//
// Created by zykov on 5/23/2026.
//

#ifndef MAXENGINE_IRESOURCESET_H
#define MAXENGINE_IRESOURCESET_H

#include <cstdint>
#include "struct/Buffer.h"
#include "IGLTexture.h"

namespace Rendering {

    class IResourceSet {
    public:
        virtual ~IResourceSet() = default;

        virtual void BindBuffer(std::uint32_t bindingPoint, BufferHandle& buffer) = 0;

        virtual void BindTexture(std::uint32_t bindingPoint, TextureHandle& texture) = 0;
    };
} // Rendering

#endif //MAXENGINE_IRESOURCESET_H
