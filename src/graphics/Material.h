//
// Created by zykov on 3/19/2026.
//

#ifndef MAXENGINE_MATERIAL_H
#define MAXENGINE_MATERIAL_H
#include "Shader.h"

struct RenderContext;

class Material {
public:
    virtual ~Material() = default;

    explicit Material(Shader* shader) {
        this->shader = shader;
    };

    Shader* shader;

    virtual void Apply();

private:
    GLint viewLocation{};
    GLint projectionLocation{};
};


#endif //MAXENGINE_MATERIAL_H