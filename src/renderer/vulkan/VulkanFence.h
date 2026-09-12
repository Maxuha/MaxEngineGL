//
// Created by zykov on 6/22/2026.
//

#ifndef MAXENGINE_VULKANFENCE_H
#define MAXENGINE_VULKANFENCE_H
#include "../IFence.h"

namespace Rendering {
    class VulkanFence : public IFence {
    public:
        VulkanFence(VkFence fence);

        ~VulkanFence() override = default;

        VkFence GetFence() const;

    private:
        VkFence fence;
    };
} // Rendering

#endif //MAXENGINE_VULKANFENCE_H
