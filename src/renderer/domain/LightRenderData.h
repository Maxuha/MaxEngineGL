//
// Created by zykov on 8/15/2026.
//

#ifndef MAXENGINE_LIGHTRENDERDATA_H
#define MAXENGINE_LIGHTRENDERDATA_H
#include "../AmbientLightData.h"

namespace Rendering {
    struct LightHandle {
        uint32_t id;
    };

    struct LightRenderData {
        BufferHandle buffer{};
        ResourceSetHandle resourceSet{};

        LightData data;
    };

    struct LightItemData {
        uint32_t offset;
        uint32_t size;
    };
} // Rendering

#endif //MAXENGINE_LIGHTRENDERDATA_H
