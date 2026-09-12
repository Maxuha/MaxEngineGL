//
// Created by zykov on 6/22/2026.
//

#ifndef MAXENGINE_ISPAWCHAIN_H
#define MAXENGINE_ISPAWCHAIN_H

#include "../src/math/Rect.h"
#include <vulkan/vulkan_core.h>


namespace Rendering {
    struct Result;
    class ISemaphore;

    class ISwapchain {
        public:
        virtual ~ISwapchain() = default;

        virtual Result AcquireNextImage(ISemaphore* semaphore, uint32_t &imageIndex) = 0;

        virtual Rect GetViewport() = 0;

        virtual VkFormat GetImageFormat() const;
    };
} // Rendering

#endif //MAXENGINE_ISPAWCHAIN_H
