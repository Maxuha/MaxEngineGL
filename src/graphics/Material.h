//
// Created by zykov on 3/19/2026.
//

#ifndef MAXENGINE_MATERIAL_H
#define MAXENGINE_MATERIAL_H
#include "Shader.h"

struct RenderContext;

class Material {
public:
    Shader* shader;
    GLuint shaderCompiled;

    Material(Shader* shader) {
        this->shader = shader;
        shaderCompiled = shader->compileShader();
    };

    virtual void Apply();

private:
    GLint viewLocation{};
    GLint projectionLocation{};

    void compileShader() const;
};


#endif //MAXENGINE_MATERIAL_H