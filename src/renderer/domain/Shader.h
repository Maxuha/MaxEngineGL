//
// Created by zykov on 3/19/2026.
//

#ifndef MAXENGINE_SHADER_H
#define MAXENGINE_SHADER_H

#include <string>
#include <unordered_map>
#include "IShader.h"
#include "../managers/IShaderManager.h"
#include "struct/ResourceSet.h"

#define MAT_COLOR_DIFFUSE

using ShaderReflectionLayout = std::unordered_map<ShaderProperty, ShaderMetaProperty>;

struct ShaderDesc {
    std::vector<char> vertexCode;
    std::vector<char> fragmentCode;
    std::vector<Rendering::ResourceSetLayoutDesc> resourceSetLayouts;
};

class Shader : public IShader {
public:
    Shader(const char *vCode, const char *fCode);

    Shader(const std::vector<char> &vCode, const std::vector<char> &fCode);

    ~Shader() override;

    void AddProperty(ShaderProperty materialProperty, ShaderMetaProperty shaderProperty) override;

    ShaderMetaProperty GetProperty(ShaderProperty property) const override;

    std::unordered_map<ShaderProperty, ShaderMetaProperty>& GetProperties() override;

    Rendering::ShaderHandle GetVHandle() const;

    Rendering::ShaderHandle GetFHandle() const;

    Rendering::ShaderHandle GetHandle() const;

private:
    std::unordered_map<ShaderProperty, ShaderMetaProperty> properties;

    Rendering::ShaderHandle handle;
    Rendering::ShaderHandle vHandle{};
    Rendering::ShaderHandle fHandle{};
};


#endif //MAXENGINE_SHADER_H
