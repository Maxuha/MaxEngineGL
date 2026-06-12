//
// Created by zykov on 5/23/2026.
//

#ifndef MAXENGINE_GLRESOURCESET_H
#define MAXENGINE_GLRESOURCESET_H
#include <unordered_map>

#include "IGLTexture.h"
#include "IResourceSet.h"
#include "struct/Buffer.h"

namespace Rendering {
    enum class ResourceType;

    struct GLResourceBinding {
        ResourceType type;
         void *resourcePtr;
        TextureHandle texture;
    };

    class GLResourceSet : public IResourceSet {
    public:
        ~GLResourceSet() override = default;

        void BindBuffer(std::uint32_t bindingPoint, BufferHandle& buffer) override;

        void BindTexture(std::uint32_t bindingPoint, TextureHandle& texture) override;

        const std::unordered_map<std::uint32_t, GLResourceBinding> &GetBindings() const;

    private:
        std::unordered_map<std::uint32_t, GLResourceBinding> bindings;
    };
} // Rendering

#endif //MAXENGINE_GLRESOURCESET_H
