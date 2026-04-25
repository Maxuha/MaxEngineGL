//
// Created by zykov on 3/19/2026.
//

#include "Shader.h"

#include <fstream>
#include <ostream>
#include <glm/glm.hpp>
#include "../IO/FileReader.h"
#include "glm/gtc/type_ptr.hpp"
#include "../math/Color.h"

void Shader::Init(const char *vertexShaderPath, const char *fragmentShaderPath) {
    const std::string vSource = FileReader::ReadFileString(vertexShaderPath);
    const std::string fSource = FileReader::ReadFileString(fragmentShaderPath);

    const char* vCode = vSource.c_str();
    const char* fCode = fSource.c_str();

    // Vertex shader
    const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vCode, nullptr);
    glCompileShader(vertexShader);

    // Fragment shader
    const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fCode, nullptr);
    glCompileShader(fragmentShader);

    // Create program and attach our shaders
    Id = glCreateProgram();
    glAttachShader(Id, vertexShader);
    glAttachShader(Id, fragmentShader);
    glLinkProgram(Id);

    // Delete shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::Enable() const {
    glUseProgram(Id);
}

void Shader::Disable() const {
    glUseProgram(0);
}

void Shader::SetUniform(const std::string& var, Matrix4x4 mat) const {
    const GLint varId = glGetUniformLocation(Id, var.c_str());

    if (varId == -1) {
        throw std::runtime_error("Invalid variable " + var);
    }

    glUniformMatrix4fv(varId, 1, false, glm::value_ptr(mat.Convert<glm::mat4>()));
}

void Shader::SetUniform(const std::string& var, Vector3 vec) const {
    const GLint varId = glGetUniformLocation(Id, var.c_str());

    if (varId == -1) {
        throw std::runtime_error("Invalid variable " + var);
    };

    glUniform3fv(varId, 1, glm::value_ptr(vec.Convert<glm::vec3>()));
}

void Shader::SetUniform(const std::string& var, const int val) const {
    const GLint varId = glGetUniformLocation(Id, var.c_str());

    if (varId == -1) {
        throw std::runtime_error("Invalid variable " + var);
    };

    glUniform1i(varId, val);
}

void Shader::SetUniform(const std::string& var, const float val) const {
    const GLint varId = glGetUniformLocation(Id, var.c_str());

    if (varId == -1) {
        throw std::runtime_error("Invalid variable " + var);
    };

    glUniform1f(varId, val);
}

void Shader::SetUniform(const std::string &var, Color color) const {
    const GLint varId = glGetUniformLocation(Id, var.c_str());

    if (varId == -1) {
        throw std::runtime_error("Invalid variable " + var);
    };

    glUniform3f(varId, color.r, color.g, color.b);
}
