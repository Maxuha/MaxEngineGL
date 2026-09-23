//
// Created by zykov on 4/8/2026.
//

#ifndef MAXENGINE_SHADERIMPORTER_H
#define MAXENGINE_SHADERIMPORTER_H
#include <string>

#include "IShaderImporter.h"
#include "../../../cmake-build-debug/_deps/spirv_reflect-src/spirv_reflect.h"


class Shader;

class ShaderImporter : public IShaderImporter {
public:
    ~ShaderImporter() override;

    Shader *Import(const std::string &shaderName) override;

private:
    void reflectShader(ShaderDesc& desc);

    void reflectStage(const std::vector<char> &code, Rendering::ShaderStageFlags stage,
                             std::unordered_map<uint32_t, std::unordered_map<uint32_t, Rendering::ResourceBindingDesc>>& aggregatedSets);

    Rendering::ResourceType MapDescriptorType(SpvReflectDescriptorType type);

};


#endif //MAXENGINE_SHADERIMPORTER_H
