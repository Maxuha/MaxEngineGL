//
// Created by zykov on 5/1/2026.
//

#include "RenderQueue.h"

namespace Rendering {

void RenderQueue::Add(const DrawCall &drawCall) {
    drawCalls.push_back(drawCall);
}

void RenderQueue::Clear() {
    drawCalls.clear();
}

std::vector<DrawCall>* RenderQueue::GetDrawCalls() {
    return &drawCalls;
}

} // namespace Rendering
