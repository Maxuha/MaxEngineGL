//
// Created by zykov on 3/19/2026.
//

#include "Shader.h"

#include <fstream>

Shader::Shader(const char *vCode, const char *fCode) {
}

Shader::Shader(const std::vector<char> &vCode, const std::vector<char> &fCode) {
    ResourceSetLayoutDesc layout_0;
    layout_0.set_index = 0;

    ResourceBindingDesc binding_0;
    binding_0.binding_slot = 0;
    binding_0.type = ResourceType::UniformBuffer;
    binding_0.stage_flags = ShaderStageFlags::Vertex;
    binding_0.count = 1;

    ResourceBindingDesc binding_3;
    binding_3.binding_slot = 1;
    binding_3.type = ResourceType::UniformBuffer;
    binding_3.stage_flags = ShaderStageFlags::Fragment;
    binding_3.count = 1;

    layout_0.bindings.push_back(binding_0);
    layout_0.bindings.push_back(binding_3);

    ResourceSetLayoutDesc layout_1;
    layout_1.set_index = 1;

    ResourceBindingDesc binding_1;
    binding_1.binding_slot = 2;
    binding_1.type = ResourceType::UniformBuffer;
    binding_1.stage_flags = ShaderStageFlags::Vertex;
    binding_1.count = 1;

    layout_1.bindings.push_back(binding_1);

    ResourceSetLayoutDesc layout_2;
    layout_2.set_index = 2;

    ResourceBindingDesc binding_2;
    binding_2.binding_slot = 4;
    binding_2.type = ResourceType::SampledImage;
    binding_2.stage_flags = ShaderStageFlags::Fragment;
    binding_2.count = 1;

    layout_2.bindings.push_back(binding_2);

    ResourceBindingDesc binding_4;
    binding_4.binding_slot = 5;
    binding_4.type = ResourceType::SampledImage;
    binding_4.stage_flags = ShaderStageFlags::Fragment;
    binding_4.count = 1;

    layout_2.bindings.push_back(binding_4);

    ResourceBindingDesc binding_5;
    binding_5.binding_slot = 3;
    binding_5.type = ResourceType::UniformBuffer;
    binding_5.stage_flags = ShaderStageFlags::Fragment;
    binding_5.count = 1;

    layout_2.bindings.push_back(binding_5);

    ShaderDesc desc;
    desc.vertexCode = vCode;
    desc.fragmentCode = fCode;
    desc.resourceSetLayouts.push_back(layout_0);
    desc.resourceSetLayouts.push_back(layout_1);
    desc.resourceSetLayouts.push_back(layout_2);

    handle = DIContainer::GetInstance().Get<IRenderer>()->CreateShader(desc);
}

Shader::~Shader() = default;

void Shader::AddProperty(const ShaderProperty materialProperty, const ShaderMetaProperty shaderProperty) {
    properties[materialProperty] = shaderProperty;
}

ShaderMetaProperty Shader::GetProperty(const ShaderProperty property) const {
    return properties.at(property);
}

std::unordered_map<ShaderProperty, ShaderMetaProperty>& Shader::GetProperties() {
    return properties;
}

ShaderHandle Shader::GetVHandle() const {
    return vHandle;
}

ShaderHandle Shader::GetFHandle() const {
    return fHandle;
}

Rendering::ShaderHandle Shader::GetHandle() const {
    return handle;
}
