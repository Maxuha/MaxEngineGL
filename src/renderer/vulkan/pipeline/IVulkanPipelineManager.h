//
// Created by zykov on 6/19/2026.
//

#ifndef MAXENGINE_IVULKANPIPELINEMANAGER_H
#define MAXENGINE_IVULKANPIPELINEMANAGER_H

namespace Rendering {
    struct VulkanPipeline;
    struct PipelineHandle;

    class IVulkanPipelineManager {
        public:
            virtual ~IVulkanPipelineManager() = default;

        VulkanPipeline GetPipeline(PipelineHandle handle);
    };
} // Rendering

#endif //MAXENGINE_IVULKANPIPELINEMANAGER_H
