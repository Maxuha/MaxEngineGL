//
// Created by zykov on 6/3/2026.
//

#ifndef MAXENGINE_PIPELINEDATA_H
#define MAXENGINE_PIPELINEDATA_H
#include "PipelineLayout.h"
#include "glad/glad.h"

namespace Rendering {
    struct PipelineData {
        uint32_t vao{};
        GLuint program{};
        GLboolean depthTestEnable{};
        GLboolean depthWriteEnable{};
        GLboolean cullEnable{};
        GLenum cullMode{};
        GLenum fillMode{};
        PipelineLayout layout =
            PipelineLayout { .items = { { 3, PipelineLayoutItem { .bindingIndex = 3 } },
            {7, PipelineLayoutItem { .bindingIndex = 7 } },
                {4, PipelineLayoutItem { .bindingIndex = 7 } }} };
    };
} // Rendering

#endif //MAXENGINE_PIPELINEDATA_H
