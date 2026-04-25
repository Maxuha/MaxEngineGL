//
// Created by zykov on 4/22/2026.
//

#include "PointLight.h"
#include "../../math/Color.h"
#include "../../graphics/Shader.h"
#include "../../components/Transform.h"

PointLight::PointLight(const Color &color, const float intensity, const float range) : LocationalLight(color, intensity),
    range(range) {
}

void PointLight::Enable(Shader &shader, int i) {
    const std::string name = "pointLights[" + std::to_string(i) + "]";

    shader.SetUniform(name + ".position", GetTransform()->position);
    shader.SetUniform(name + ".intensity", intensity);
    shader.SetUniform(name + ".range", range);
    shader.SetUniform(name + ".constant", constant);
    shader.SetUniform(name + ".linear", linear);
    shader.SetUniform(name + ".quadratic", quadratic);
    shader.SetUniform(name + ".color", color);
}
