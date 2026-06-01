//
// Created by zykov on 5/22/2026.
//

#include "PipelineManager.h"

#include "../domain/GLPipeline.h"

namespace Rendering {
    PipelineManager::PipelineManager() = default;

    PipelineManager::~PipelineManager() {
        PipelineManager::Clear();
    };

    IPipeline * PipelineManager::GetOrCreatePipeline(const std::vector<ShaderInput> &shaders, const RenderState &renderState) {
        const PipelineKey key{ shaders, renderState };

        auto it = cache.find(key);
        if (it != cache.end()) {
            return it->second.get();
        }

        IPipeline* newPipeline = new GLPipeline();

        newPipeline->Compile(shaders, renderState);

        //cache[key] = std::move(newPipeline);

        return newPipeline;
    }

    void PipelineManager::Clear() {
        cache.clear();
    }
} // Rendering