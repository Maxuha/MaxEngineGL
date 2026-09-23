//
// Created by zykov on 4/22/2026.
//

#include "PointLight.h"

PointLight::PointLight() {
    lightType = Rendering::LightType::Point;
    handle = DIContainer::GetInstance().Get<IRenderer>()->CreateLight(lightType);
}

float PointLight::GetRange() const {
    return range;
}

void PointLight::SetRange(float range) {
    this->range = range;
}
