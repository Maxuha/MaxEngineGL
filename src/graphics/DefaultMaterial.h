//
// Created by zykov on 3/20/2026.
//

#ifndef MAXENGINE_SIMPLEMATERIAL_H
#define MAXENGINE_SIMPLEMATERIAL_H
#include "Material.h"
#include "Texture.h"
#include "../math/Color.h"

class GameObject;

class DefaultMaterial : public Material {
public:
    explicit DefaultMaterial(Shader *shader, Texture *diffuse, Texture *specular, Color color);

    void Enable(std::vector<Light *>& lights, Camera &camera, GameObject &gameObject) override;

    void Disable() override;

private:
    Color color;;
    Texture *diffuseTexture;
    Texture *specularTexture;

    int diffuseLocation = 0;
    int specularLocation = 1;
    float shininess = 64.0f;
};


#endif //MAXENGINE_SIMPLEMATERIAL_H
