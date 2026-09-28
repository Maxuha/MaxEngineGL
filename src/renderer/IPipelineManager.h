//
// Created by zykov on 6/26/2026.
//

#ifndef MAXENGINE_IPIPELINEMANAGER_H
#define MAXENGINE_IPIPELINEMANAGER_H

namespace Rendering {
    struct ShaderHandle;
    struct RenderPassHandle;
    struct PipelineStateDesc;

    struct PipelineHandle {
        uint32_t Id;
    };

    class IPipelineManager {
    public:
        virtual ~IPipelineManager() = default;
    };
} // Rendering

#endif //MAXENGINE_IPIPELINEMANAGER_H
