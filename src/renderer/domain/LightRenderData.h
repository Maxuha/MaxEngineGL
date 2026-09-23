//
// Created by zykov on 8/15/2026.
//

#ifndef MAXENGINE_LIGHTRENDERDATA_H
#define MAXENGINE_LIGHTRENDERDATA_H
#include "IBufferManager.h"
#include "../AmbientLightData.h"
#include "../IResourceSetManager.h"

enum class LightType;

namespace Rendering {
    struct LightHandle {
        uint32_t id;
    };

    enum class LightType {
        Ambient,
        Directional,
        Point,
        Spot
    };

    struct ShadowRenderData {
        BufferHandle buffer{};
        ResourceSetHandle resourceSet{};

        Matrix4x4 space{};
    };

    struct LightRenderData {
        BufferHandle buffer{};
        ResourceSetHandle resourceSet{};

        LightData data;
    };

    struct LightItemData {
        LightType type;
        uint32_t index;
        uint32_t lastIndexForPointLights;
        uint32_t lastIndexForSpotLights;
    };
} // Rendering

#endif //MAXENGINE_LIGHTRENDERDATA_H
