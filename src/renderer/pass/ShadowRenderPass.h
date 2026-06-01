//
// Created by zykov on 5/14/2026.
//

#ifndef MAXENGINE_SHADOWRENDERPASS_H
#define MAXENGINE_SHADOWRENDERPASS_H
#include "IRenderPass.h"
#include "../IRenderDevice.h"
#include "../domain/buffer/frame/FrameBuffer.h"

namespace Rendering {
    class ShadowRenderPass : public IRenderPass {
    public:
        ShadowRenderPass(FrameBuffer& shadowFrameBuffer, ShaderInput& shadowShader);

        ~ShadowRenderPass() override;

        void Execute(IRenderDevice& renderDevice, RenderQueue& renderQueue, LightData lightData, MeshManager& meshManager) const override;

    private:
        FrameBuffer *shadowFrameBuffer;
        ShaderInput *shadowShader;

    };
} // Rendering

#endif //MAXENGINE_SHADOWRENDERPASS_H
