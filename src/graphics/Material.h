//
// Created by zykov on 3/19/2026.
//

#ifndef MAXENGINE_MATERIAL_H
#define MAXENGINE_MATERIAL_H
#include "Shader.h"

class GameObject;
struct RenderContext;

class Material {
public:
    virtual ~Material() = default;

    explicit Material(Shader* shader) {
        this->shader = shader;
    };

    virtual void Enable(const RenderContext &context, GameObject* gameObject);
    virtual void Disable();

private:
    GLint viewLocation{};
    GLint projectionLocation{};

protected:
    Shader* shader;

};


#endif //MAXENGINE_MATERIAL_H