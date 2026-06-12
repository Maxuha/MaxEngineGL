//
// Created by zykov on 4/22/2026.
//

#include "SpotLight.h"

float SpotLight::GetInnerAngle() const {
    return innerAngle;
}

float SpotLight::GetOuterAngle() const {
    return outerAngle;
}

float SpotLight::GetRange() const {
    return range;
}
