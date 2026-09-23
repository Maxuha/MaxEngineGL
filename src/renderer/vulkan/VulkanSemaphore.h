//
// Created by zykov on 6/22/2026.
//

#ifndef MAXENGINE_VULKANSEMAPHORE_H
#define MAXENGINE_VULKANSEMAPHORE_H
#include "../ISemaphore.h"

namespace Rendering {
    class VulkanSemaphore : public ISemaphore {
    public:
        VulkanSemaphore() = default;

        VulkanSemaphore(VkSemaphore semaphore);

        ~VulkanSemaphore() override = default;

        VkSemaphore GetSemaphore() const;

    private:
        VkSemaphore semaphore;
    };
} // Rendering

#endif //MAXENGINE_VULKANSEMAPHORE_H
