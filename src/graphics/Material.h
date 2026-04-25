//
// Created by zykov on 3/19/2026.
//

#ifndef MAXENGINE_MATERIAL_H
#define MAXENGINE_MATERIAL_H
#include "Shader.h"
#include "../gameObject/Camera.h"
#include "../gameObject/light/Light.h"

class GameObject;
struct RenderContext;

class Material {
public:
    explicit Material(Shader* shader);

    virtual ~Material() = default;

    virtual void Enable(std::vector<Light*>& lights, Camera& camera, GameObject& gameObject);
    virtual void Disable();

private:
    GLint viewLocation{};
    GLint projectionLocation{};

protected:
    Shader* shader;

};


#endif //MAXENGINE_MATERIAL_H