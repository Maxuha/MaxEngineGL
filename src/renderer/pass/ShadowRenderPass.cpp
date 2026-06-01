//
// Created by zykov on 5/14/2026.
//

#include "ShadowRenderPass.h"
#include "../RenderQueue.h"

namespace Rendering {
    ShadowRenderPass::ShadowRenderPass(FrameBuffer& shadowFrameBuffer, ShaderInput& shadowShader) {
        this->shadowFrameBuffer = &shadowFrameBuffer;
        this->shadowShader = &shadowShader;
    }

    ShadowRenderPass::~ShadowRenderPass() = default;

    void ShadowRenderPass::Execute(IRenderDevice& renderDevice, RenderQueue& renderQueue, const LightData lightData, MeshManager& meshManager) const {
        // shadowFrameBuffer->Bind();
        //
        // const Viewport viewport{ 0.0f, 0.0f, static_cast<float>(shadowFrameBuffer->GetWidth()), static_cast<float>(shadowFrameBuffer->GetHeight()) };
        // const Color clearColor{ 0.1f, 0.1f, 0.1f, 1.0f };
        // constexpr auto flags = ClearFlags::Depth;
        //
        // renderDevice.SetViewport(viewport);
        // renderDevice.BeginRenderPass(flags, clearColor, 1.0f);
        // renderDevice.SetCullMode(CullMode::Front);
        //
        // renderDevice.SetPipelineState(PipelineHandle{.Id = 1});
        // renderDevice.PushConstants("space", lightData.directionalLight.space);
        // for (const auto& drawCall: *renderQueue.GetDrawCalls()) {
        //     renderDevice.PushConstants("model", drawCall.model);
        //     renderDevice.DrawIndexed(drawCall.mesh->GetIndexCount());
        // }
        // shadowFrameBuffer->Unbind();
    }
} // Rendering