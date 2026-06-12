//
// Created by zykov on 6/2/2026.
//

#ifndef MAXENGINE_SPIRVIMPORTER_H
#define MAXENGINE_SPIRVIMPORTER_H
#include <cstdint>
#include <unordered_map>

#include "IShaderImporter.h"


struct UniformBlockReflection {
    std::string name;
    uint32_t binding;
    size_t totalSize;
    std::unordered_map<std::string, size_t> memberOffsets;
};

struct TextureReflection {
    std::string name;
    uint32_t binding;
};

struct PipelineReflection {
    std::string vertexGLSL;
    std::string fragmentGLSL;

    std::unordered_map<std::string, UniformBlockReflection> uniformBlocks;
    std::unordered_map<std::string, TextureReflection> textures;
};

class SpirvImporter : IShaderImporter {
public:
    Shader* Import(const std::string& shaderName) override;
};


#endif //MAXENGINE_SPIRVIMPORTER_H
