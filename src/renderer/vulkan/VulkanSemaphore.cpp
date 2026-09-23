//
// Created by zykov on 6/22/2026.
//

#include "VulkanSemaphore.h"

namespace Rendering {
    VulkanSemaphore::VulkanSemaphore(const VkSemaphore semaphore) {
        this->semaphore = semaphore;
    }

    VkSemaphore VulkanSemaphore::GetSemaphore() const {
        return semaphore;
    }
} // Rendering