//
// Created by zykov on 6/1/2026.
//

#ifndef MAXENGINE_IWINDOWCONTEXT_H
#define MAXENGINE_IWINDOWCONTEXT_H

namespace Rendering {
    class IWindowContext {
    public:
        virtual ~IWindowContext() = default;
        virtual void* GetNativeHandle() = 0;
    };
} // Rendering

#endif //MAXENGINE_IWINDOWCONTEXT_H
