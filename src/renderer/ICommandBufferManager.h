//
// Created by zykov on 6/25/2026.
//

#ifndef MAXENGINE_ICOMMANDBUFFERMANAGER_H
#define MAXENGINE_ICOMMANDBUFFERMANAGER_H


namespace Rendering {
    struct CommandBufferHandle;
    struct CommandPoolHandle;

    struct CommandBufferHandle {
        uint32_t Id;
    };

    class ICommandBufferManager {
    public:
        virtual ~ICommandBufferManager() = default;
    };
} // Rendering

#endif //MAXENGINE_ICOMMANDBUFFERMANAGER_H
