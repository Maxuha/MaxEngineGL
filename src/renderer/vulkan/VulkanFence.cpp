//
// Created by zykov on 6/22/2026.
//

#include "VulkanFence.h"

namespace Rendering {
    VulkanFence::VulkanFence(const VkFence fence) {
        this->fence = fence;
    }

    VkFence VulkanFence::GetFence() const {
        return fence;
    }
} // Rendering