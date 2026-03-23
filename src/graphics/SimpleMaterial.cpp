//
// Created by zykov on 3/20/2026.
//

#include "SimpleMaterial.h"

SimpleMaterial::SimpleMaterial(Shader* shader) : Material(shader) {
}

void SimpleMaterial::Apply() {
    Material::Apply();

    shader->SetColor("color", color);
}
