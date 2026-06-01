//
// Created by zykov on 5/21/2026.
//

#ifndef MAXENGINE_PIPELINESTATEDESC_H
#define MAXENGINE_PIPELINESTATEDESC_H
#include <string>

#include "VertexLayout.h"

namespace Rendering {
    struct DepthStencilDesc {
        bool depthTestEnable = true;
        bool depthWriteEnable = true;
    };

    struct RasterizerDesc {
        bool cullEnable = true;
        enum class CullMode { Front, Back } cullMode = CullMode::Back;
        enum class FillMode { Solid, Wireframe } fillMode = FillMode::Solid;
    };

    struct PipelineStateDesc {
        std::string vertexShaderSource;
        std::string fragmentShaderSource;

        ShaderId shaderId;

        VertexLayout vertexLayout;

        DepthStencilDesc depthStencilState;
        RasterizerDesc rasteizerState;
    };
} // Rendering

#endif //MAXENGINE_PIPELINESTATEDESC_H
