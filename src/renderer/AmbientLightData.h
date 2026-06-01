//
// Created by zykov on 5/3/2026.
//

#ifndef MAXENGINE_AMBIENTLIGHTDATA_H
#define MAXENGINE_AMBIENTLIGHTDATA_H
#include "../math/Matrix4x4.h"
#include "../math/Vector4.h"


namespace Rendering {
    struct AmbientLightData {
        Vector4 colorIntensity{};
    };

    struct DirectionalLightData {
        // Matrix4x4 projection{};
        // Matrix4x4 view{};
        Matrix4x4 space{};
        Vector4 colorIntensity{};
        Vector4 direction{};
    };

    struct PointLightData {
        Vector4 colorIntensity{};
        Vector4 positionRange{};
    };

    struct SpotLightData {
        Vector4 colorIntensity{};
        Vector4 positionRange{};
        Vector4 direction{};
        Vector4 coneAngle{};
    };

    struct LightData {
        AmbientLightData ambientLight;
        DirectionalLightData directionalLight;
        PointLightData pointLights[8];
        SpotLightData spotLights[8];
    };
} // namespace Rendering


#endif //MAXENGINE_AMBIENTLIGHTDATA_H
