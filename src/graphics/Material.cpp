//
// Created by zykov on 3/19/2026.
//

#include "Material.h"
#include "RenderContext.h"
#include "../components/Transform.h"
#include "../gameObject/GameObject.h"

void Material::Enable(const RenderContext &context, GameObject* gameObject) {
    shader->Enable();

    shader->SetUniform("view", context.viewMatrix);
    shader->SetUniform("projection", context.projectionMatrix);
    shader->SetUniform("model", gameObject->GetComponent<Transform>()->GetModelMatrix());

    shader->SetUniform("lightDirection", context.lightDirection);
    shader->SetUniform("lightColor", context.lightColor);
    shader->SetUniform("lightIntensity", context.lightIntensity);
}

void Material::Disable() {
    shader->Disable();
}
