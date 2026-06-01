//
// Created by zykov on 5/15/2026.
//

#include "MainRenderPass.h"

#include "../CameraData.h"
#include "../IRenderDevice.h"
#include "../RenderQueue.h"
#include "../../di/DIContainer.h"
#include "../domain/GLCommandBuffer.h"
#include "../domain/buffer/BufferDesc.h"

namespace Rendering {
    MainRenderPass::MainRenderPass(GLTexture &texture) : texture(texture) {
      //  auto* renderDevice = DIContainer::GetInstance().Get<IRenderDevice>();

       // commandBuffer = new GLCommandBuffer(*renderDevice, 100000);

        // BufferDesc bufferDesc;
        // bufferDesc.size = sizeof(CameraData);
        // bufferDesc.index = 0;
        // bufferDesc.usage = BufferUsage::Uniform;
        // cameraBufferId = renderDevice->CreateBuffer(bufferDesc, nullptr);
        //
        // // TO DO fix bufferId
        // renderDevice->BindUniformBuffer(cameraBufferId, 0);
    }

    MainRenderPass::~MainRenderPass() = default;

    void MainRenderPass::Execute(IRenderDevice &renderDevice, RenderQueue &renderQueue, const LightData lightData,
                                 MeshManager &meshManager) const {
       //  constexpr Viewport viewport{0.0f, 0.0f, 2560.0f, 1440.0f};
       //  const Color clearColor{0.1f, 0.1f, 0.1f, 1.0f};
       //  const ClearFlags flags = ClearFlags::Color | ClearFlags::Depth;
       //
       //  renderDevice.SetViewport(viewport);
       //  renderDevice.BeginRenderPass(flags, clearColor, 1.0f);
       // // renderDevice.SetCullMode(CullMode::None);
       //
       //  AttachmentDescription attachmentDescription{};
       //  attachmentDescription.loadOp = AttachmentLoadOp::CLEAR;
       //  attachmentDescription.storeOp = AttachmentStoreOp::DONT_CARE;
       //  attachmentDescription.type = AttachmentType::COLOR;
       //
       //  const std::vector attachmentDescriptions = {attachmentDescription};
       //
       //  commandBuffer->BeginRenderPass(attachmentDescriptions, 0, viewport.width, viewport.height, 1);
       //
       //  for (const auto &drawCall: *renderQueue.GetDrawCalls()) {
       //      BufferDesc desc;
       //      desc.size = sizeof(uint8_t);
       //      desc.usage = BufferUsage::Uniform;
       //
       //      BufferId b = DIContainer::GetInstance().Get<IRenderDevice>()->CreateBuffer(desc,std::vector<uint8_t> { 32 }.data() );
       //
       //      IResourceSet* resourceSet = renderDevice.CreateResourceSet();
       //      resourceSet->BindTexture(0, *drawCall.material->GetTexture("diffuse"));
       //      resourceSet->BindTexture(1, *drawCall.material->GetTexture("specular"));
       //      resourceSet->BindBuffer(3, b);
       //      resourceSet->Update();
       //
       //      commandBuffer->BindPipeline(*drawCall.shader->pipeline);
       //      commandBuffer->BindVertexBuffer(drawCall.mesh->vertexBuffer);
       //      commandBuffer->BindIndexBuffer(drawCall.mesh->indexBuffer);
       //
       //      renderDevice.PushConstants("model", drawCall.transform->GetWorldMatrix());
       //
       //      commandBuffer->BindResourceSet(0, *resourceSet);
       //      commandBuffer->Draw(drawCall.mesh->GetIndexCount());
       //
       //      DIContainer::GetInstance().Get<IRenderDevice>()->SubmitCommandBuffer(commandBuffer);
       //      DIContainer::GetInstance().Get<IRenderDevice>()->Present();
       //
       //      commandBuffer->ExecuteAll();

        ///------///

            // DIContainer::GetInstance().Get<IRenderDevice>()->SubmitCommandBuffer(commandBuffer);
            // DIContainer::GetInstance().Get<IRenderDevice>()->Present();
            // commandBuffer->ExecuteAll();

            // renderDevice.SetPipelineState(PipelineHandle{.Id = 0});
            // renderDevice.BindVertexBuffer(drawCall.mesh->vertex);
            // renderDevice.BindIndexBuffer(drawCall.mesh->index);

            // TextureId diffuse = drawCall.material->GetTexture("diffuse")->GetId();
            // TextureId specular = drawCall.material->GetTexture("specular")->GetId();
            //
            // ResourceSet m_resourceSet;
            // m_resourceSet.bindings.push_back({
            //     .slot = 0,
            //     .type = ResourceType::SamplerTexture,
            //     .resourceId = diffuse.Id
            // });
            //
            // m_resourceSet.bindings.push_back({
            //     .slot = 1,
            //     .type = ResourceType::SamplerTexture,
            //     .resourceId = specular.Id
            // });
            // renderDevice.BindResourceSet(m_resourceSet);

            // //drawCall.material->GetBuffer()->UploadSubData(drawCall.material->GetBufferData().data(), 0, 4);
            // renderDevice.PushConstants("lightSpaceMatrix", lightData.directionalLight.space);
            // renderDevice.PushConstants("depthMap", 2);
            // renderDevice.PushConstants("model", drawCall.transform->GetWorldMatrix());
            // //texture.Bind(2);
            // renderDevice.DrawIndexed(drawCall.mesh->GetIndexCount());
        // }

      //  commandBuffer->EndRenderPass(attachmentDescriptions);
        // DIContainer::GetInstance().Get<IRenderDevice>()->SubmitCommandBuffer(commandBuffer);
        // DIContainer::GetInstance().Get<IRenderDevice>()->Present();
        // commandBuffer->ExecuteAll();
    }
} // Rendering
