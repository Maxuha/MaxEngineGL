//
// Created by zykov on 9/18/2026.
//

#include "LightItemRenderData.h"

namespace Rendering {
    void LightItemRenderData::SetPosition(const Vector3 &position) {
        this->position = position;
        isDirty = true;
    }

    void LightItemRenderData::SetColor(const Color &color) {
        this->color = color;
        isDirty = true;
    }

    void LightItemRenderData::SetIntensity(const float intensity) {
        this->intensity = intensity;
        isDirty = true;
    }

    void LightItemRenderData::SetRange(const float range) {
        this->range = range;
        isDirty = true;
    }

    void LightItemRenderData::SetInnerConeAngle(const float innerConeAngle) {
        this->innerConeAngle = innerConeAngle;
        isDirty = true;
    }

    void LightItemRenderData::SetOuterConeAngle(const float outerConeAngle) {
        this->outerConeAngle = outerConeAngle;
        isDirty = true;
    }
} // Rendering