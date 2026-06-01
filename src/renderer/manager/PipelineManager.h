//
// Created by zykov on 5/22/2026.
//

#ifndef MAXENGINE_PIPELINEMANAGER_H
#define MAXENGINE_PIPELINEMANAGER_H
#include "IPipelineManager.h"
#include "../domain/IPipeline.h"

namespace Rendering {
    class PipelineManager : public IPipelineManager {
    public:
        PipelineManager();

        ~PipelineManager() override;

        IPipeline *GetOrCreatePipeline(const std::vector<ShaderInput> &shaders, const RenderState &renderState) override;

        void Clear() override;

    private:
        std::unordered_map<PipelineKey, std::unique_ptr<IPipeline>, PipelineKeyHash> cache;
    };
} // Rendering

#endif //MAXENGINE_PIPELINEMANAGER_H
