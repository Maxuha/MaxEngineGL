//
// Created by zykov on 4/8/2026.
//

#include "ShaderImporter.h"
#include "../graphics/Shader.h"

Shader* ShaderImporter::Import(std::string shaderName) {
    auto *litShader = new Shader(
        (std::string(ASSETS_ROOT) + "/shaders/" + shaderName + ".vert").c_str(),
        (std::string(ASSETS_ROOT) + "/shaders/" + shaderName + ".frag").c_str()
    );
    return litShader;
}
