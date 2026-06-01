//
// Created by zykov on 5/26/2026.
//

#ifndef MAXENGINE_IBUFFER_H
#define MAXENGINE_IBUFFER_H

namespace Rendering {
    class IBuffer {
    public:
        virtual ~IBuffer() = default;

        virtual BufferId GetId() = 0;

        virtual void UpdateData(void* data) = 0;
    };
} // Rendering

#endif //MAXENGINE_IBUFFER_H
