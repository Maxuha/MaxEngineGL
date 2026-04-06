//
// Created by zykov on 3/20/2026.
//

#ifndef MAXENGINE_SIMPLEMATERIAL_H
#define MAXENGINE_SIMPLEMATERIAL_H
#include "Material.h"
#include "Texture.h"
#include "../math/Color.h"

class GameObject;

class DefaultMaterial: public Material {

public:
    explicit DefaultMaterial(Shader* shader);

    Color color{};
    Texture* texture;

    void Enable(const RenderContext &context, GameObject* gameObject) override;
    void Disable() override;

};


#endif //MAXENGINE_SIMPLEMATERIAL_H