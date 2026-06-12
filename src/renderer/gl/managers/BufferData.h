//
// Created by zykov on 6/12/2026.
//

#ifndef MAXENGINE_BUFFERDATA_H
#define MAXENGINE_BUFFERDATA_H
#include "glad/glad.h"

namespace Rendering {
    struct BufferData {
        GLuint id{};
    };

    struct FrameBufferData {
        GLuint id{};
        float width{};
        float height{};
    };
} // Rendering

#endif //MAXENGINE_BUFFERDATA_H
