//
// Created by zykov on 5/21/2026.
//

#ifndef MAXENGINE_RESOURCESET_H
#define MAXENGINE_RESOURCESET_H
#include <cstdint>
#include <vector>

namespace Rendering {
    struct ResourceSetLayoutDesc;
}

namespace Rendering {
    struct ResourceBinding;

    enum class ShaderStageFlags : uint32_t {
        None = 0,
        Vertex = 1 << 0,
        Fragment = 1 << 1,
        Compute = 1 << 2,
        AllStages = 0x7FFFFFFF
    };

    inline ShaderStageFlags operator|(ShaderStageFlags a, ShaderStageFlags b) {
        return static_cast<ShaderStageFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    enum class ResourceType : uint8_t {
        UniformBuffer, // Constant Buffer (UBO)
        StorageBuffer, // Structured / Raw Buffer (SSBO)
        SampledImage, // Texture View (SRV)
        StorageImage, // Read/Write Texture (UAV)
        Sampler // Sampler State
    };

    struct ResourceBindingDesc {
        uint32_t binding_slot = 0; // (layout(binding = X))
        ResourceType type = ResourceType::UniformBuffer;
        uint32_t count = 1;
        ShaderStageFlags stage_flags = ShaderStageFlags::None;
        bool is_dynamic = false; // (UBO/SSBO)
    };

    // (Descriptor Set Layout / Bind Group Layout)
    struct ResourceSetLayoutDesc {
        uint32_t set_index = 0; //(layout(set = X)
        std::vector<ResourceBindingDesc> bindings;

        // void add_binding(const uint32_t slot, const ResourceType type, const ShaderStageFlags stages, const uint32_t count = 1) {
        //     bindings.push_back({slot, type, count, stages, false});
        // }
    };

    inline bool operator==(const ResourceBindingDesc &a, const ResourceBindingDesc &b) {
        return a.binding_slot == b.binding_slot &&
               a.type == b.type &&
               a.count == b.count &&
               a.stage_flags == b.stage_flags &&
               a.is_dynamic == b.is_dynamic;
    }

    inline bool operator==(const ResourceSetLayoutDesc &a, const ResourceSetLayoutDesc &b) {
        if (a.set_index != b.set_index || a.bindings.size() != b.bindings.size()) {
            return false;
        }

        return a.bindings == b.bindings;
    }

    template<typename T>
    inline void hash_combine(size_t &seed, const T &value) {
        std::hash<T> hasher;
        seed ^= hasher(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    inline size_t compute_layout_hash(ResourceSetLayoutDesc layout) {
        std::sort(layout.bindings.begin(), layout.bindings.end(),
                  [](const ResourceBindingDesc &a, const ResourceBindingDesc &b) {
                      return a.binding_slot < b.binding_slot;
                  });

        size_t seed = 0;

        hash_combine(seed, layout.set_index);

        for (const auto &binding: layout.bindings) {
            hash_combine(seed, binding.binding_slot);
            hash_combine(seed, static_cast<uint8_t>(binding.type));
            hash_combine(seed, binding.count);
            hash_combine(seed, static_cast<uint32_t>(binding.stage_flags));
            hash_combine(seed, binding.is_dynamic);
        }

        return seed;
    }

    struct ResourceBinding {
        std::uint32_t slot;
        std::uint32_t binding;
        ResourceType type;
        uint32_t resourceId;
    };

    struct ResourceSet {
        std::vector<ResourceBinding> bindings;
    };
}

template<>
struct std::hash<Rendering::ResourceSetLayoutDesc> {
    size_t operator()(const Rendering::ResourceSetLayoutDesc& layout) const noexcept {
        return Rendering::compute_layout_hash(layout);
    }
};

#endif //MAXENGINE_RESOURCESET_H
