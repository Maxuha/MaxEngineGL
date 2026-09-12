//
// Created by zykov on 6/26/2026.
//

#ifndef MAXENGINE_OGLFRAMEBUFFERMANAGER_H
#define MAXENGINE_OGLFRAMEBUFFERMANAGER_H
#include "../IFrameBufferManager.h"
#include "../RenderContext.h"

namespace Rendering {
    struct FrameBufferHandle;
    struct TextureHandle;
    struct RenderPassHandle;

    struct OGLFrameBuffer {
        GLuint Framebuffer;
    };

    class OGLFrameBufferManager : public IFrameBufferManager {
    public:
        explicit OGLFrameBufferManager(RenderContext &context);

        ~OGLFrameBufferManager() override;

      //  FrameBufferHandle CreateFramebuffer(RenderPassHandle renderPass, TextureHandle texture) override;

        OGLFrameBuffer GetFrameBuffer(FrameBufferHandle frameBuffer) const;

    private:
        RenderContext* context;

        std::vector<OGLFrameBuffer> frameBuffers;

    };
} // Rendering

#endif //MAXENGINE_OGLFRAMEBUFFERMANAGER_H
