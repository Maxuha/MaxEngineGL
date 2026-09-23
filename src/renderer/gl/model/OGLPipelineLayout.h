//
// Created by zykov on 6/10/2026.
//

#ifndef MAXENGINE_PIPELINELAYOUTDATA_H
#define MAXENGINE_PIPELINELAYOUTDATA_H
#include "../../domain/struct/ResourceSet.h"


namespace Rendering {
    enum class ResourceType : uint8_t;
    struct OGLResourceSet;

    struct OGLDescriptorSetLayoutItem {
        uint32_t binding_slot = 0;
        ResourceType type = ResourceType::UniformBuffer;
        ShaderStageFlags stage_flags = ShaderStageFlags::None;
    };

    struct OGLDescriptorSetLayout {
        uint32_t set_index = 0;
        std::vector<OGLDescriptorSetLayoutItem> bindings;
    };

    struct OGLPipelineLayout {
        std::vector<OGLDescriptorSetLayout> resources;
    };
} // Rendering

#endif //MAXENGINE_PIPELINELAYOUTDATA_H
