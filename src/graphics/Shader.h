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
    void LoadShader(const char *vertexShaderPath, const char *fragmentShaderPath);
    GLuint compileShader();

    void SetMat4(const std::string& var, Matrix4x4 mat) const;
    void SetVec3(const std::string& var, Vector3 vec) const;
    void SetFloat(const std::string& var, float val) const;
    void SetColor(const std::string& var, Color color) const;


private:
    GLuint cachedShader = 0;
    std::string vertexShaderSource;
    std::string fragmentShaderSource;

};


#endif //MAXENGINE_SHADER_H