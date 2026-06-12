//
// Created by zykov on 3/19/2026.
//

#ifndef MAXENGINE_SHADER_H
#define MAXENGINE_SHADER_H

#include <string>
#include <unordered_map>
#include "IShader.h"
#include "GLPipeline.h"

#define MAT_COLOR_DIFFUSE

using ShaderReflectionLayout = std::unordered_map<ShaderProperty, ShaderMetaProperty>;

class Shader : public IShader {
public:
    Shader(const char *vCode, const char *fCode);

    ~Shader() override;

    void AddProperty(ShaderProperty materialProperty, ShaderMetaProperty shaderProperty) override;

    ShaderMetaProperty GetProperty(ShaderProperty property) const override;

    std::vector<Rendering::ShaderInput> GetShader() override;

    std::unordered_map<ShaderProperty, ShaderMetaProperty>& GetProperties() override;

private:
    Rendering::ShaderInput vShader{};
    Rendering::ShaderInput fShader{};

    std::unordered_map<ShaderProperty, ShaderMetaProperty> properties;
};


#endif //MAXENGINE_SHADER_H
