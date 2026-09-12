//
// Created by zykov on 6/4/2026.
//

#ifndef MAXENGINE_MATERIALDATA_H
#define MAXENGINE_MATERIALDATA_H


namespace Rendering {
    struct PipelineHandle;

    struct MaterialData {
        ShaderHandle shader{};
        PipelineHandle pipeline{};
        BufferHandle ubo{};
        std::unordered_map<uint32_t, TextureHandle> textures;
        ResourceSetHandle resourceSet;
    };
} // Rendering

#endif //MAXENGINE_MATERIALDATA_H
