//
// Created by zykov on 6/10/2026.
//

#ifndef MAXENGINE_PIPELINELAYOUTDATA_H
#define MAXENGINE_PIPELINELAYOUTDATA_H
#include "glm/fwd.hpp"

namespace Rendering {
    struct PipelineLayoutItem {
        int bindingIndex;
    };

    struct PipelineLayout {
        std::unordered_map<uint32_t, PipelineLayoutItem> items;
    };
} // Rendering

#endif //MAXENGINE_PIPELINELAYOUTDATA_H
