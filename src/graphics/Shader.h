//
// Created by zykov on 3/19/2026.
//

#ifndef MAXENGINE_SHADER_H
#define MAXENGINE_SHADER_H

#include <string>
#include "../math/Color.h"
#include "../math/Matrix4x4.h"
#include "../math/Vector3.h"
#include "glad/glad.h"


class Shader {
public:
    Shader(const char *vertexShaderPath, const char *fragmentShaderPath) {
        Init(vertexShaderPath, fragmentShaderPath);
    }

    ~Shader() {
        glDeleteProgram(Id);
    }

    void Use() const;

    void SetUniform(const std::string& var, Matrix4x4 mat) const;
    void SetUniform(const std::string& var, Vector3 vec) const;
    void SetUniform(const std::string& var, float val) const;
    void SetUniform(const std::string& var, Color color) const;


private:
    void Init(const char *vertexShaderPath, const char *fragmentShaderPath);
    GLuint Id = 0;
};


#endif //MAXENGINE_SHADER_H