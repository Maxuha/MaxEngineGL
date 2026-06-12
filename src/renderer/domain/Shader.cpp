//
// Created by zykov on 3/19/2026.
//

#include "Shader.h"

#include <fstream>
#include "GLPipeline.h"


Shader::Shader(const char *vCode, const char *fCode) {
    Rendering::ShaderInput vShader = {};
    vShader.Source = vCode;
    vShader.Type = Rendering::ShaderType::Vertex;

    this->vShader = vShader;

    Rendering::ShaderInput fShader = {};
    fShader.Source = fCode;
    fShader.Type = Rendering::ShaderType::Fragment;

    this->fShader = fShader;
}

Shader::~Shader() = default;

void Shader::AddProperty(const ShaderProperty materialProperty, const ShaderMetaProperty shaderProperty) {
    properties[materialProperty] = shaderProperty;
}

ShaderMetaProperty Shader::GetProperty(const ShaderProperty property) const {
    return properties.at(property);
}

std::vector<Rendering::ShaderInput> Shader::GetShader() {
    return std::vector { vShader, fShader };
}

std::unordered_map<ShaderProperty, ShaderMetaProperty>& Shader::GetProperties() {
    return properties;
}
