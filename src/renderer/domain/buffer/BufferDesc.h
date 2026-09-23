//
// Created by zykov on 5/7/2026.
//

#ifndef MAXENGINE_BUFFERDESC_H
#define MAXENGINE_BUFFERDESC_H
#include "BufferUsage.h"

namespace Rendering {

struct BufferDesc {
    unsigned size = 0;
    unsigned stride = 0;
    unsigned index = 0;
    uint32_t set = 0;
    uint32_t binding = 0;
    BufferUsage usage {};
    bool isDynamic = false;
};

} // namespace Rendering

#endif //MAXENGINE_BUFFERDESC_H
