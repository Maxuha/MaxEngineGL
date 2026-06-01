//
// Created by zykov on 5/1/2026.
//

#ifndef MAXENGINE_RENDERQUEUE_H
#define MAXENGINE_RENDERQUEUE_H
#include <vector>

#include "DrawCall.h"


namespace Rendering {

class RenderQueue {
public:
    void Add(const DrawCall &drawCall);
    void Clear();
    std::vector<DrawCall>* GetDrawCalls();

private:
    std::vector<DrawCall> drawCalls;
};

} // namespace Rendering


#endif //MAXENGINE_RENDERQUEUE_H
