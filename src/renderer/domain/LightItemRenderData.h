//
// Created by zykov on 9/18/2026.
//

#ifndef MAXENGINE_LIGHT_H
#define MAXENGINE_LIGHT_H
#include "LightRenderData.h"
#include "../../math/Color.h"

namespace Rendering {
    class LightItemRenderData {
    public:
        LightItemRenderData() = default;

        LightType type;
        Vector3 position;
        Color color;
        float intensity{};
        float innerConeAngle{};
        float outerConeAngle{};
        float range{};

        LightHandle handle{};

        bool isDirty = false;

        void SetPosition(const Vector3& position);

        void SetColor(const Color& color);

        void SetIntensity(float intensity);

        void SetRange(float range);

        void SetInnerConeAngle(float innerConeAngle);

        void SetOuterConeAngle(float outerConeAngle);
    };
} // Rendering

#endif //MAXENGINE_LIGHT_H
