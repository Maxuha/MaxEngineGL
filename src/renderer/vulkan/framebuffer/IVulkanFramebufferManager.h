//
// Created by zykov on 6/19/2026.
//

#ifndef MAXENGINE_IVULKANFRAMEBUFFER_H
#define MAXENGINE_IVULKANFRAMEBUFFER_H

namespace Rendering {
    struct VulkanFrameBuffer;
    struct FrameBufferHandle;
}

namespace Rendeing {
    class IVulkanFramebufferManager {
    public:
        virtual ~IVulkanFramebufferManager() = default;

        virtual Rendering::VulkanFrameBuffer GetFrameBuffer(Rendering::FrameBufferHandle handle) = 0;
    };
} // Rendeing

#endif //MAXENGINE_IVULKANFRAMEBUFFER_H
