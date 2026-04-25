//
// Created by zykov on 4/22/2026.
//

#include "SpotLight.h"
#include "../../components/Transform.h"
#include "../../graphics/Shader.h"

SpotLight::SpotLight(const Color color, const float intensity, const float range, const float innerAngle,
                     const float outerAngle) : LocationalLight(color, intensity),
                                               range(range), innerAngle(innerAngle), outerAngle(outerAngle) {
}

void SpotLight::Enable(Shader &shader, int i) {
    shader.SetUniform("spotLight.position", GetTransform()->position);
    shader.SetUniform("spotLight.direction", GetTransform()->Forward());
    shader.SetUniform("spotLight.range", range);
    shader.SetUniform("spotLight.intensity", intensity);
    shader.SetUniform("spotLight.constant", constant);
    shader.SetUniform("spotLight.linear", linear);
    shader.SetUniform("spotLight.quadratic", quadratic);
    shader.SetUniform("spotLight.cutOff", innerAngle);
    shader.SetUniform("spotLight.outerCutOff", outerAngle);
    shader.SetUniform("spotLight.color", color);
}
