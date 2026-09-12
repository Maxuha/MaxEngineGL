//
// Created by zykov on 6/26/2026.
//

#include "OGLShaderManager.h"

#include <iostream>

namespace Rendering {
    OGLShaderManager::OGLShaderManager() {
    }

    OGLShaderManager::~OGLShaderManager() {
    }

    ShaderHandle OGLShaderManager::CreateShader(const ShaderType type, const std::vector<char> &code) {
        GLuint shaderId = 0;
        switch (type) {
            case ShaderType::Vertex:
                shaderId = glCreateShader(GL_VERTEX_SHADER);
                break;
            case ShaderType::Fragment:
                shaderId = glCreateShader(GL_FRAGMENT_SHADER);
                break;
        }

        glShaderBinary(1, &shaderId, GL_SHADER_BINARY_FORMAT_SPIR_V, code.data(), code.size());

        glSpecializeShader(shaderId, "main", 0, nullptr, nullptr);

        int success;
        char infoLog[1024];
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shaderId, 1024, nullptr, infoLog);
            throw std::runtime_error("SPIR-V Shader Compilation Error" + std::string(infoLog));
        }

        const ShaderHandle shaderHandle = { .Id = shaders.size() };

        const auto shader = OGLShader { .handle = shaderId } ;

        shaders.push_back(shader);

        return shaderHandle;
    }

    ShaderHandle OGLShaderManager::CreateShader(const ShaderType type, const char *code) {
        GLuint shaderId = 0;
        switch (type) {
            case ShaderType::Vertex:
                shaderId = glCreateShader(GL_VERTEX_SHADER);
                break;
            case ShaderType::Fragment:
                shaderId = glCreateShader(GL_FRAGMENT_SHADER);
                break;
        }

        glShaderSource(shaderId, 1, &code, nullptr);
        glCompileShader(shaderId);

        int success;
        GLchar infoLog[512];

        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);

        if (!success) {
            glGetShaderInfoLog(shaderId, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        const ShaderHandle shaderHandle = { .Id = shaders.size() };

        const auto shader = OGLShader { .handle = shaderId } ;

        shaders.push_back(shader);

        return shaderHandle;
    }

    OGLShader OGLShaderManager::GetShader(const ShaderHandle handle) const {
        return shaders[handle.Id];
    }
} // Rendering