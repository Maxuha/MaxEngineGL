//
// Created by zykov on 5/14/2026.
//

#ifndef MAXENGINE_IRENDERPASS_H
#define MAXENGINE_IRENDERPASS_H
#include "../AmbientLightData.h"

namespace Rendering {
    class MeshManager;
    class RenderQueue;
    class IRenderDevice;

    class IRenderPass {
    public:
        virtual ~IRenderPass() = default;

        virtual void Execute(IRenderDevice& renderDevice, RenderQueue& renderQueue, LightData lightData, MeshManager& meshManager) const = 0;
    };
} // Rendering

#endif //MAXENGINE_IRENDERPASS_H
