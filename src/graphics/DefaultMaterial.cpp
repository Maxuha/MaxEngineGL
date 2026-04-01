//
// Created by zykov on 3/20/2026.
//

#include "DefaultMaterial.h"

DefaultMaterial::DefaultMaterial(Shader* shader) : Material(shader), color() {
}

void DefaultMaterial::Apply() {
    Material::Apply();

    shader->SetUniform("color", color);
}
