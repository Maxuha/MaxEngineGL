//
// Created by zykov on 3/20/2026.
//

#include "DefaultMaterial.h"
#include "../gameObject/GameObject.h"

DefaultMaterial::DefaultMaterial(Shader* shader) : Material(shader) {
}

void DefaultMaterial::Enable(const RenderContext &context, GameObject* gameObject) {
    Material::Enable(context, gameObject);
    shader->SetUniform("color", color);
    texture->Bind();
}

void DefaultMaterial::Disable() {
    Material::Disable();
}
