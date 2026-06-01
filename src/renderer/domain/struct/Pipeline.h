//
// Created by zykov on 5/21/2026.
//

#ifndef MAXENGINE_PIPELINESTATE_H
#define MAXENGINE_PIPELINESTATE_H
#include "../../../graphics/IShader.h"

namespace Rendering {
    struct Pipeline {
        ShaderId shaderId;
        GLuint vao;
        std::vector<uint32_t> vertexStrides;
    };
}


#endif //MAXENGINE_PIPELINESTATE_H
