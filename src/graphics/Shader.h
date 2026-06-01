//
// Created by zykov on 3/19/2026.
//

#ifndef MAXENGINE_SHADER_H
#define MAXENGINE_SHADER_H

#include <string>

#include "IShader.h"
#include "Material.h"
#include "../math/Vector3.h"
#include "../renderer/domain/IPipeline.h"
#include "../renderer/domain/struct/PipelineHandle.h"
#include "glad/glad.h"

using ShaderReflectionLayout = std::unordered_map<std::string, ShaderProperty>;

class Shader : public IShader {
public:
    // Shader(const char *vertexShaderPath, const char *fragmentShaderPath) {
    //     Init(vertexShaderPath, fragmentShaderPath);
    // }

    Shader(const char *vCode, const char *fCode);

    ~Shader() override {
        glDeleteProgram(Id);
    }

    ShaderId GetId() const override {
        return id;
    }

    void Enable() const override;

    void Disable() const override;

    void SetUniform(const std::string &var, Matrix4x4 mat) const override;

    void SetUniform(const std::string &var, Vector3 vec) const override;

    void SetUniform(const std::string &var, int val) const override;

    void SetUniform(const std::string &var, float val) const override;

    void SetUniform(const std::string &var, Color color) const override;

    void SetUniform(const std::string &var, glm::mat4 color) const;

    void SetUniform(const std::string &var, Texture& texture) const override;

    void Init(const char *vertexShaderPath, const char *fragmentShaderPath);

    void AddProperty(const std::string &name, ShaderProperty property) override;

    ShaderProperty GetProperty(const std::string &name) const override;

    Rendering::PipelineHandle pso{};
    //Rendering::IPipeline* pipeline;

    Rendering::IPipeline* pipeline;
    Rendering::PipelineHandle pipelineHandle;

private:
    ShaderId id{};
    GLuint Id = 0;

    const char *vCode{};
    const char *fCode{};

    std::unordered_map<std::string, ShaderProperty> properties;
};


#endif //MAXENGINE_SHADER_H
