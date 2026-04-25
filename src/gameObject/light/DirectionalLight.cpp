//
// Created by zykov on 3/17/2026.
//

#include "DirectionalLight.h"
#include "../../components/Transform.h"
#include "../../graphics/Shader.h"


DirectionalLight::DirectionalLight(const Color color, const float intensity) : Light(color, intensity) {
}

void DirectionalLight::Enable(Shader &shader, int i) {
    shader.SetUniform("directionalLight.direction",GetTransform()->Forward());
    shader.SetUniform("directionalLight.color", color);
    shader.SetUniform("directionalLight.intensity", intensity);
}
