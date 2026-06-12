//
// Created by zykov on 4/22/2026.
//

#include "PointLight.h"

float PointLight::GetRange() const {
    return range;
}

void PointLight::SetRange(float range) {
    this->range = range;
}
