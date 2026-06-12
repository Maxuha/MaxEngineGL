//
// Created by zykov on 6/4/2026.
//

#ifndef MAXENGINE_MATERIALDATA_H
#define MAXENGINE_MATERIALDATA_H
#include "PipelineHandle.h"

namespace Rendering {
    struct MaterialData {
        PipelineHandle pipeline{};
        BufferHandle ubo{};
        std::unordered_map<uint32_t, TextureHandle> textures;
    };
} // Rendering

#endif //MAXENGINE_MATERIALDATA_H
