//
// Created by zykov on 5/15/2026.
//

#ifndef MAXENGINE_MAINRENDERPASS_H
#define MAXENGINE_MAINRENDERPASS_H
#include "IRenderPass.h"
#include "../domain/GLTexture.h"
#include "../domain/struct/BufferId.h"


namespace Rendering {
    class ICommandBuffer;

    class MainRenderPass : public IRenderPass{
    public:
        explicit MainRenderPass(GLTexture& texture);
        ~MainRenderPass() override;

        void Execute(IRenderDevice& renderDevice, RenderQueue& renderQueue, LightData lightData, MeshManager& meshManager) const override;

        ICommandBuffer* commandBuffer;
    private:
        GLTexture& texture;
        BufferId cameraBufferId{};

    };
} // Rendering

#endif //MAXENGINE_MAINRENDERPASS_H
