//
// Created by zykov on 3/19/2026.
//

#include "Shader.h"


Shader::Shader(const std::vector<char> &vCode, const std::vector<char> &fCode, const ShaderDesc &desc) {
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
