//
// Created by zykov on 6/22/2026.
//

#ifndef MAXENGINE_ILOGICALDEVICE_H
#define MAXENGINE_ILOGICALDEVICE_H

namespace Rendering {
    class IFence;
    class ISwapchain;
    class ISemaphore;

    struct Result {
        bool Success;
        std::string Message;
    };

    class ILogicalDevice {
    public:
        virtual ~ILogicalDevice() = default;
        // virtual Result AcquireNextImage(const ISwapchain &swapchain, const ISemaphore &semaphore, uint32_t& imageIndex) = 0;
        // virtual Result WaitIdle(const IFence& fence) = 0;
        // virtual ISemaphore* CreateSemaphor() = 0;
        // virtual IFence* CreateFence() = 0;
    };
} // Rendering

#endif //MAXENGINE_ILOGICALDEVICE_H
