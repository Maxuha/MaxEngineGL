//
// Created by zykov on 5/21/2026.
//

#ifndef MAXENGINE_PIPELINESTATEDESC_H
#define MAXENGINE_PIPELINESTATEDESC_H
#include <string>

#include "VertexLayout.h"

namespace Rendering {
    enum class CullMode { Front, Back };
    enum class FillMode { Solid, Wireframe };

    struct DepthStencilDesc {
        bool depthTestEnable = true;
        bool depthWriteEnable = true;
    };

    struct RasterizerDesc {
        bool cullEnable = true;
        CullMode cullMode = CullMode::Back;
        FillMode fillMode = FillMode::Solid;
    };

    struct PipelineStateDesc {
        VertexLayout vertexLayout;

        DepthStencilDesc depthStencilState;
        RasterizerDesc rasterizerState;
    };
} // Rendering

#endif //MAXENGINE_PIPELINESTATEDESC_H
