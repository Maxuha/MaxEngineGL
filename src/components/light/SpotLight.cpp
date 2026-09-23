//
// Created by zykov on 4/22/2026.
//

#include "SpotLight.h"

SpotLight::SpotLight() {
    lightType = Rendering::LightType::Spot;
    handle = DIContainer::GetInstance().Get<IRenderer>()->CreateLight(lightType);
}

float SpotLight::GetInnerAngle() const {
    return innerAngle;
}

float SpotLight::GetOuterAngle() const {
    return outerAngle;
}

float SpotLight::GetRange() const {
    return range;
}

void SpotLight::SetInnerAngle(const float innerAngle) {
    this->innerAngle = innerAngle;
}

void SpotLight::SetOuterAngle(const float outerAngle) {
    this->outerAngle = outerAngle;
}

void SpotLight::SetRange(const float range) {
    this->range = range;
}
