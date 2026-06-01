//
// Created by zykov on 4/30/2026.
//

#include "Renderer.h"
#include "CameraData.h"
#include "gl/GLRenderDevice.h"
#include "LightRenderer.h"
#include "../di/DIContainer.h"
#include "../graphics/MeshPrimitives.h"
#include "domain/GLCommandBuffer.h"
#include "domain/IWindowContext.h"
#include "domain/buffer/BufferDesc.h"
#include "domain/buffer/frame/FrameBuffer.h"
#include "pass/MainRenderPass.h"
#include "pass/ShadowRenderPass.h"

class AssetManager;

namespace Rendering {
    Renderer::Renderer(const RenderConfig &renderConfig, IWindowContext &context) : renderDevice(nullptr), mainShader(
            nullptr) {
        switch (renderConfig.api) {
            case RenderAPI::OpenGL: renderDevice = new GLRenderDevice(context);
                renderQueue = new RenderQueue();
                break;
            default: std::cerr << "Render API " << renderConfig.api << "is not supported yet";
                break;
        }

        commandBuffer = new GLCommandBuffer(100000);

        lightRenderer = new LightRenderer(*renderDevice);

        BufferDesc bufferDesc;
        bufferDesc.size = sizeof(CameraData);
        bufferDesc.index = 0;
        bufferDesc.usage = BufferUsage::Uniform;

        cameraBufferId = this->renderDevice->CreateBuffer(bufferDesc, nullptr);

        renderDevice->BindUniformBuffer(cameraBufferId, 0);

    }

    Renderer::~Renderer() = default;

    PipelineHandle Renderer::CreatePipeline(const std::vector<ShaderInput> &shaders, const RenderState &renderState) {
        depthBuffer = new FrameBuffer();
        depthTexture = new GLTexture(4096, 4096);
        depthBuffer->BindTexture(*depthTexture);

        return renderDevice->CreatePipeline(shaders, renderState);
    }

    TextureId Renderer::CreateTexture(void *data, TextureCreateRequest& textureCreateDesc) {
        // TextureCreateRequest desc;
        // desc.Width = width;
        // desc.Height = height;
        // desc.Format = TextureFormat::DEPTH;
        // desc.MipLevels = 1;
        return renderDevice->CreateTexture(textureCreateDesc, data);
    }

    void Renderer::UpdateTextureFilterMode(TextureId &textureId, const TextureWrap textureWrap) {
        renderDevice->UpdateTextureFilterMode(textureId, textureWrap);
    }

    Mesh *Renderer::CreateMesh(const std::span<const Vertex> vertices, const std::span<const uint32_t> indices) {
        BufferDesc bufferDesc;
        bufferDesc.size = vertices.size_bytes();
        bufferDesc.index = 0;
        bufferDesc.usage = BufferUsage::Vertex;

        const BufferId vertexBuffer = renderDevice->CreateBuffer(bufferDesc, vertices.data());

        bufferDesc.size = indices.size() * sizeof(uint32_t);
        bufferDesc.index = 1;
        bufferDesc.usage = BufferUsage::Index;

        const BufferId indexBuffer = renderDevice->CreateBuffer(bufferDesc, indices.data());

        const auto mesh = new Mesh(MeshId{.Id = 0}, vertices, indices);
        mesh->vertexBuffer = vertexBuffer;
        mesh->indexBuffer = indexBuffer;
        return mesh;
    }

    void Renderer::BeginFrame(Camera &camera, Shader *shader) {
        activeCamera = &camera;
        mainShader = shader;

        CameraData data{
            .view = activeCamera->GetTransform()->LookAt(),
            .projection = activeCamera->GetProjection(),
            .position = Vector4(camera.GetTransform()->position, 0),
        };

        renderDevice->UpdateBuffer(cameraBufferId, &data, 0, sizeof(CameraData));
    }

    void Renderer::Submit(Scene &scene) {
        const std::vector<MeshRenderer *> &renderers = scene.GetComponents<MeshRenderer>();

        constexpr Viewport viewport{0.0f, 0.0f, 2560.0f, 1440.0f};
        const ::Color clearColor{0.1f, 0.1f, 0.1f, 1.0f};

        AttachmentDescription attachmentDescription{};
        attachmentDescription.loadOp = AttachmentLoadOp::CLEAR;
        attachmentDescription.storeOp = AttachmentStoreOp::DONT_CARE;
        attachmentDescription.type = AttachmentType::COLOR | AttachmentType::DEPTH;

        // shadow

         commandBuffer->BeginRenderPass(attachmentDescription, depthBuffer->GetId(), viewport.width, viewport.height, clearColor);

         for (const auto &renderer: renderers) {
             const DrawCall drawCall{
                 .key = {},
                 .meshId = renderer->mesh->GetId(),
                 .shaderId = renderer->material->GetShader()->GetId(),
                 .model = renderer->GetGameObject()->GetTransform()->GetWorldMatrix(),
                 .mesh = renderer->mesh,
                 .material = renderer->material,
                 .transform = renderer->GetGameObject()->GetTransform(),
                 .shader = renderer->material->GetShader(),
             };

             BufferDesc desc;
             desc.size = sizeof(uint8_t);
             desc.usage = BufferUsage::Uniform;

        //     BufferId b = renderDevice->CreateBuffer(desc, std::vector<uint8_t>{32}.data());

             IResourceSet *resourceSet = renderDevice->CreateResourceSet();
             resourceSet->BindTexture(0, *drawCall.material->GetTexture("diffuse"));
             resourceSet->BindTexture(1, *drawCall.material->GetTexture("specular"));
            // resourceSet->BindBuffer(3, b);
             resourceSet->Update();

             commandBuffer->BindPipeline(dynamic_cast<Shader *>(drawCall.shader)->pipelineHandle);
             commandBuffer->BindVertexBuffer(drawCall.mesh->vertexBuffer);
             commandBuffer->BindIndexBuffer(drawCall.mesh->indexBuffer);
             commandBuffer->PushConstants("model", drawCall.transform->GetWorldMatrix());
             commandBuffer->BindResourceSet(0, *resourceSet);
             commandBuffer->Draw(drawCall.mesh->GetIndexCount());
         }

         commandBuffer->EndRenderPass(attachmentDescription);

         renderDevice->SubmitCommandBuffer(commandBuffer);

         renderDevice->Present();

         commandBuffer->ExecuteAll();

        // main

        commandBuffer->BeginRenderPass(attachmentDescription, 0, viewport.width, viewport.height, clearColor);

        for (const auto &renderer: renderers) {
            const DrawCall drawCall{
                .key = {},
                .meshId = renderer->mesh->GetId(),
                .shaderId = renderer->material->GetShader()->GetId(),
                .model = renderer->GetGameObject()->GetTransform()->GetWorldMatrix(),
                .mesh = renderer->mesh,
                .material = renderer->material,
                .transform = renderer->GetGameObject()->GetTransform(),
                .shader = renderer->material->GetShader(),
            };

            BufferDesc desc;
            desc.size = sizeof(uint8_t);
            desc.usage = BufferUsage::Uniform;

            //BufferId b = renderDevice->CreateBuffer(desc, std::vector<uint8_t>{32}.data());

            IResourceSet *resourceSet = renderDevice->CreateResourceSet();
            resourceSet->BindTexture(0, *drawCall.material->GetTexture("diffuse"));
            resourceSet->BindTexture(1, *drawCall.material->GetTexture("specular"));
            resourceSet->BindTexture(2, *depthTexture);
            // resourceSet->BindBuffer(3, b);
            resourceSet->Update();

            commandBuffer->BindPipeline(dynamic_cast<Shader *>(drawCall.shader)->pipelineHandle);
            commandBuffer->BindVertexBuffer(drawCall.mesh->vertexBuffer);
            commandBuffer->BindIndexBuffer(drawCall.mesh->indexBuffer);
            commandBuffer->PushConstants("model", drawCall.transform->GetWorldMatrix());
            commandBuffer->BindResourceSet(0, *resourceSet);
            commandBuffer->Draw(drawCall.mesh->GetIndexCount());
        }

        commandBuffer->EndRenderPass(attachmentDescription);

        renderDevice->SubmitCommandBuffer(commandBuffer);

        renderDevice->Present();

        commandBuffer->ExecuteAll();

        //

        lightRenderer->LoadLights(scene);
        lightRenderer->EnableLights();
    }

    void Renderer::Render() {
        // renderQueue.Sort();
        // renderDevice->Present();
    }

    void Renderer::EndFrame() {
        renderQueue->Clear();
        lightRenderer->Clear();
        //  renderPasses.clear();
    }
} // namespace Rendering
