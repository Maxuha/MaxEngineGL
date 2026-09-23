//
// Created by zykov on 6/26/2026.
//

#ifndef MAXENGINE_ICOMMANDPOOLMANAGER_H
#define MAXENGINE_ICOMMANDPOOLMANAGER_H

namespace Rendering {
    struct CommandPoolHandle {
        uint32_t Id;
    };

    enum class CommandPoolType {
        Graphics,
        Compute,
        Transfer
    };

    class ICommandPoolManager {
    public:
        virtual ~ICommandPoolManager() = default;

        virtual CommandPoolHandle CreateCommandPool(CommandPoolType type) = 0;
    };
} // Rendering

#endif //MAXENGINE_ICOMMANDPOOLMANAGER_H
