//
// Created by zykov on 5/7/2026.
//

#ifndef MAXENGINE_BUFFERSLOT_H
#define MAXENGINE_BUFFERSLOT_H
#include "BufferDesc.h"

namespace Rendering {

struct BufferSlot {
    unsigned id;
    BufferDesc desc;
    bool occupied;
};

} // namespace Rendering

#endif //MAXENGINE_BUFFERSLOT_H
