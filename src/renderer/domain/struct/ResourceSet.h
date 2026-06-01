//
// Created by zykov on 5/21/2026.
//

#ifndef MAXENGINE_RESOURCESET_H
#define MAXENGINE_RESOURCESET_H
#include <cstdint>
#include <vector>

namespace Rendering {
    enum class ResourceType {
        SamplerTexture,
        UniformBuffer
    };

    struct ResourceBinding {
        std::uint32_t slot;
        ResourceType type;
        uint32_t resourceId;
    };

    struct ResourceSet {
        std::vector<ResourceBinding> bindings;
    };
}


#endif //MAXENGINE_RESOURCESET_H
