//
// Created by zykov on 5/23/2026.
//

#include "GLResourceSet.h"
#include "struct/ResourceSet.h"

namespace Rendering {
    class IBuffer;

    void GLResourceSet::BindBuffer(const std::uint32_t bindingPoint, BufferHandle& buffer) {
        GLResourceBinding binding{};
        binding.type = ResourceType::UniformBuffer;
        binding.resourcePtr = &buffer;
        bindings[bindingPoint] = binding;
    }

    void GLResourceSet::BindTexture(const std::uint32_t bindingPoint, TextureHandle &texture) {
        GLResourceBinding binding{};
        binding.type = ResourceType::SamplerTexture;
        binding.resourcePtr = &texture;
        binding.texture = texture;
        bindings[bindingPoint] = binding;
    }

    const std::unordered_map<uint32_t, GLResourceBinding> &GLResourceSet::GetBindings() const {
        return bindings;
    }
} // Rendering
