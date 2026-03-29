//
// Created by zykov on 3/19/2026.
//

#include "Shader.h"

#include <fstream>
#include <ostream>
#include <glm/glm.hpp>

#include "../utils/FileReader.h"
#include "glm/gtc/type_ptr.hpp"

void Shader::LoadShader(const char *vertexShaderPath, const char *fragmentShaderPath) {
    vertexShaderSource = FileReader::LoadFile(vertexShaderPath);
    fragmentShaderSource = FileReader::LoadFile(fragmentShaderPath);
}

GLuint Shader::compileShader() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const GLchar* vertexShaderPtr = vertexShaderSource.c_str();
    glShaderSource(vertexShader, 1, &vertexShaderPtr, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar* fragmentShaderPtr = fragmentShaderSource.c_str();
    glShaderSource(fragmentShader, 1, &fragmentShaderPtr, nullptr);
    glCompileShader(fragmentShader);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    cachedShader = program;

    return program;
}

void Shader::Use() const {
    glUseProgram(cachedShader);
}

void Shader::SetMat4(const std::string& var, Matrix4x4 mat) const {
    const GLint varId = glGetUniformLocation(cachedShader, var.c_str());

    if (varId == -1) {
        throw std::runtime_error("Invalid variable " + var);
    }

    glUniformMatrix4fv(varId, 1, false, glm::value_ptr(mat.Convert<glm::mat4>()));
}

void Shader::SetVec3(const std::string& var, Vector3 vec) const {
    const GLint varId = glGetUniformLocation(cachedShader, var.c_str());

    if (varId == -1) {
        throw std::runtime_error("Invalid variable " + var);
    };

    glUniform3fv(varId, 1, glm::value_ptr(vec.Convert<glm::vec3>()));
}

void Shader::SetFloat(const std::string& var, const float val) const {
    const GLint varId = glGetUniformLocation(cachedShader, var.c_str());

    if (varId == -1) {
        throw std::runtime_error("Invalid variable " + var);
    };

    glUniform1f(varId, val);
}

void Shader::SetColor(const std::string &var, Color color) const {
    const GLint varId = glGetUniformLocation(cachedShader, var.c_str());

    if (varId == -1) {
        throw std::runtime_error("Invalid variable " + var);
    };

    glUniform3f(varId, color.r, color.g, color.b);
}
