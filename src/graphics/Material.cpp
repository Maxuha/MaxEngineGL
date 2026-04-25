//
// Created by zykov on 3/19/2026.
//

#include "Material.h"

#include "RenderContext.h"
#include "../components/Transform.h"
#include "../gameObject/GameObject.h"

Material::Material(Shader *shader) : shader(shader) {
}

void Material::Enable(std::vector<Light *>& lights, Camera& camera, GameObject& gameObject) {
    shader->Enable();

    shader->SetUniform("viewPos", camera.GetTransform()->position);
    shader->SetUniform("view", camera.GetTransform()->LookAt());
    shader->SetUniform("projection", camera.GetProjection());
    shader->SetUniform("model", gameObject.GetTransform()->GetWorldMatrix());

    for (unsigned int i = 0; i < lights.size(); i++) {
        lights[i]->Enable(*shader, i);
    }
}

void Material::Disable() {
    shader->Disable();
}
