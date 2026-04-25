//
// Created by zykov on 3/20/2026.
//

#include "DefaultMaterial.h"
#include "../gameObject/GameObject.h"

DefaultMaterial::DefaultMaterial(Shader *shader, Texture *diffuse, Texture *specular,
                                 const Color color) : Material(shader),
                                                      color(color),
                                                      diffuseTexture(diffuse),
                                                      specularTexture(specular) {
}

void DefaultMaterial::Enable(std::vector<Light *> &lights, Camera &camera, GameObject &gameObject) {
    Material::Enable(lights, camera, gameObject);

    shader->SetUniform("material.diffuse", diffuseLocation);
    shader->SetUniform("material.specular", specularLocation);
    shader->SetUniform("material.shininess", shininess);

    diffuseTexture->Activate(diffuseLocation);
    diffuseTexture->Bind();
    specularTexture->Activate(specularLocation);
    specularTexture->Bind();
}

void DefaultMaterial::Disable() {
    Material::Disable();
}
