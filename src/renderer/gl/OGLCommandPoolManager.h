//
// Created by zykov on 6/26/2026.
//

#ifndef MAXENGINE_OGLCOMMANDPOOLMANAGER_H
#define MAXENGINE_OGLCOMMANDPOOLMANAGER_H

#include "../src/renderer/ICommandPoolManager.h"


namespace Rendering {
    class OGLCommandPoolManager : public ICommandPoolManager {
    public:
        explicit OGLCommandPoolManager();
        ~OGLCommandPoolManager() override;

        CommandPoolHandle CreateCommandPool(CommandPoolType type) override;
    };
} // Rendering

#endif //MAXENGINE_OGLCOMMANDPOOLMANAGER_H
