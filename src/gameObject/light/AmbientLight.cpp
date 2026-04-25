//
// Created by zykov on 4/24/2026.
//

#include "AmbientLight.h"
#include "../../graphics/Shader.h"

AmbientLight::AmbientLight(const Color color, const float intensity) : Light(color, intensity) {
}

void AmbientLight::Enable(Shader &shader, int i) {
    shader.SetUniform("ambientLight.color", color);
    shader.SetUniform("ambientLight.intensity", intensity);
}
