//
// Created by zykov on 4/30/2026.
//

#include "Renderer.h"
#include <cstring>
#include "CameraData.h"
#include "gl/GLRenderDevice.h"
#include "LightRenderer.h"
#include "../Engine.h"
#include "domain/MeshPrimitives.h"
#include "gl/GLCommandBuffer.h"
#include "domain/IWindowContext.h"
#include "domain/buffer/BufferDesc.h"
#include "domain/struct/MaterialData.h"

class AssetManager;

namespace Rendering {
    Renderer::Renderer(const RenderConfig &renderConfig, IWindowContext &context) : litShader(renderConfig.litShader),
        renderDevice(nullptr) {
        switch (renderConfig.api) {
            case RenderAPI::OpenGL: renderDevice = new GLRenderDevice(context);
                break;
            default: std::cerr << "Render API " << renderConfig.api << "is not supported yet";
                break;
        }

        commandBuffer = new GLCommandBuffer(1024 * 1024);

        lightRenderer = new LightRenderer(*renderDevice);

        BufferDesc bufferDesc;
        bufferDesc.size = sizeof(CameraData);
        bufferDesc.index = 0;
        bufferDesc.usage = BufferUsage::Uniform;

        cameraBufferId = this->renderDevice->CreateBuffer(bufferDesc, nullptr);

        renderDevice->BindUniformBuffer(cameraBufferId, 0);

        TextureCreateRequest request;
        request.Format = TextureFormat::DEPTH;
        request.Width = 2048;
        request.Height = 2048;
        request.MipLevels = 0;

        depthTexture = this->renderDevice->CreateTexture(request, nullptr);
        depthBuffer = this->renderDevice->CreateFrameBuffer(depthTexture);

        VertexAttribute position_attribute;
        position_attribute.format = VertexFormat::Float3;
        position_attribute.location = 0;
        position_attribute.offset = 0;
        position_attribute.bufferIndex = 0;

        VertexBinding buffer_binding;
        buffer_binding.bufferIndex = 0;
        buffer_binding.stride = sizeof(Vertex);
        buffer_binding.stepRate = VertexStepRate::PerVertex;

        VertexLayout vertex_layout;
        vertex_layout.attributes.push_back(position_attribute);
        vertex_layout.bindings.push_back(buffer_binding);

        PipelineStateDesc desc;
        desc.depthStencilState.depthTestEnable = true;
        desc.depthStencilState.depthWriteEnable = true;
        desc.rasterizerState.cullEnable = true;
        desc.rasterizerState.cullMode = CullMode::Front;
        desc.rasterizerState.fillMode = FillMode::Solid;
        desc.vertexLayout = vertex_layout;

        shadowPipeline = this->renderDevice->CreatePipeline(renderConfig.shadowShader->GetShader(), desc);
    }

    Renderer::~Renderer() = default;

    TextureHandle Renderer::CreateTexture(void *data, TextureCreateRequest &textureCreateDesc) {
        return renderDevice->CreateTexture(textureCreateDesc, data);
    }

    MaterialHandle Renderer::CreateMaterial(IShader &shader, const PipelineStateDesc &pipelineStateDesc) {
        BufferDesc bufferDesc;
        bufferDesc.size = 32;
        bufferDesc.index = 2;
        bufferDesc.usage = BufferUsage::Uniform;

        const std::vector<ShaderInput> shader_inputs = shader.GetShader();

        MaterialData data;
        data.pipeline = renderDevice->CreatePipeline(shader_inputs, pipelineStateDesc);
        data.ubo = renderDevice->CreateBuffer(bufferDesc, nullptr);

        const MaterialHandle handle { .Id = static_cast<uint32_t>(materials.size()) };

        materials.push_back(data);

        return handle;
    }

    void Renderer::MaterialSetTexture(const MaterialHandle materialHandle, const std::string &slot,
                                      const TextureHandle textureId) {
        if (slot == "diffuse") {
            materials[materialHandle.Id].textures[0] = textureId;
        } else if (slot == "specular") {
            materials[materialHandle.Id].textures[1] = textureId;
        }
    }

    void Renderer::UpdateMaterialData(const MaterialHandle handle, const size_t offset, const size_t size, void *data) {
        UpdateBufferData(materials[handle.Id].ubo, offset, size, data);
    }

    void Renderer::UpdateTextureFilterMode(TextureHandle &textureId, const TextureWrap textureWrap) {
        renderDevice->UpdateTextureFilterMode(textureId, textureWrap);
    }

    void Renderer::UpdateBufferData(const BufferHandle bufferId, const size_t offset, const size_t size, void *data) {
        renderDevice->UpdateBuffer(bufferId, data, offset, size);
    }

    Mesh *Renderer::CreateMesh(const std::span<const Vertex> vertices, const std::span<const uint32_t> indices) {
        BufferDesc bufferDesc;
        bufferDesc.size = vertices.size_bytes();
        bufferDesc.index = 0;
        bufferDesc.usage = BufferUsage::Vertex;

        const BufferHandle vertexBuffer = renderDevice->CreateBuffer(bufferDesc, vertices.data());

        bufferDesc.size = indices.size() * sizeof(uint32_t);
        bufferDesc.index = 1;
        bufferDesc.usage = BufferUsage::Index;

        const BufferHandle indexBuffer = renderDevice->CreateBuffer(bufferDesc, indices.data());

        const auto mesh = new Mesh(vertices, indices);
        mesh->vertexBuffer = vertexBuffer;
        mesh->indexBuffer = indexBuffer;
        return mesh;
    }

    void Renderer::BeginFrame(Camera &camera) {
        activeCamera = &camera;
    }

    void Renderer::Submit(Scene &scene) {
        const std::vector<MeshRenderer *> &renderers = scene.GetComponents<MeshRenderer>();
        const std::vector<Light *> &lights = scene.GetComponents<Light>();

        constexpr Viewport viewport{0.0f, 0.0f, 2560.0f, 1440.0f};
        const ::Color clearColor{0.1f, 0.1f, 0.1f, 1.0f};

        AttachmentDescription attachmentDescription1{};
        attachmentDescription1.loadOp = AttachmentLoadOp::CLEAR;
        attachmentDescription1.storeOp = AttachmentStoreOp::STORE;
        attachmentDescription1.type = AttachmentType::DEPTH;

        AttachmentDescription attachmentDescription2{};
        attachmentDescription2.loadOp = AttachmentLoadOp::CLEAR;
        attachmentDescription2.storeOp = AttachmentStoreOp::DONT_CARE;
        attachmentDescription2.type = AttachmentType::COLOR | AttachmentType::DEPTH;

        // lights
        lightRenderer->LoadLights(scene);
        lightRenderer->EnableLights();

        // shadow pass

        commandBuffer->BeginRenderPass(attachmentDescription1, depthBuffer.Id, clearColor);

        for (const auto &renderer: renderers) {
            const DrawCall drawCall{
                .key = {},
                .meshId = renderer->mesh->GetId(),
                .model = renderer->GetGameObject()->GetTransform()->GetWorldMatrix(),
                .mesh = renderer->mesh,
                .material = renderer->material,
                .transform = renderer->GetGameObject()->GetTransform(),
                .shader = renderer->material->GetShader(),
            };

            commandBuffer->BindPipeline(shadowPipeline);
            commandBuffer->BindVertexBuffer(drawCall.mesh->vertexBuffer);
            commandBuffer->BindIndexBuffer(drawCall.mesh->indexBuffer);
            commandBuffer->PushConstants(3, drawCall.transform->GetWorldMatrix());
          //  commandBuffer->PushConstants(11, lights[1]->GetGameObject()->GetTransform()->Perspective(60.0f, 1.0f, 1.0f, 40.0f) * lights[1]->GetGameObject()->GetTransform()->LookAt());
            commandBuffer->PushConstants(4,  lights[1]->GetGameObject()->GetTransform()->Othographic(-100.0f, 100.0f, -100.0f, 100.0f, 1.0f, 100.0f) * lights[1]->GetGameObject()->GetTransform()->LookAt());
            commandBuffer->Draw(drawCall.mesh->GetIndexCount());
        }

        commandBuffer->EndRenderPass(attachmentDescription1);

       // renderDevice->SubmitCommandBuffer(commandBuffer);

         // Engine::SaveDepthTextureToFile(1, 4096, 4096, "depth.png");
         //
         // return;
        // main pass

        // -1 is window
        commandBuffer->BeginRenderPass(attachmentDescription2, -1, viewport.width, viewport.height, clearColor);

        CameraData cameraData{
            .view = activeCamera->GetTransform()->LookAt(),
            .projection = activeCamera->GetProjection(),
            .position = Vector4(activeCamera->GetTransform()->position, 0),
        };

        commandBuffer->UpdateBuffer(cameraBufferId, &cameraData, 0, sizeof(CameraData));

        for (const auto &renderer: renderers) {
            if (!renderer) continue;

            const DrawCall drawCall{
                .key = {},
                .meshId = renderer->mesh->GetId(),
                .model = renderer->GetGameObject()->GetTransform()->GetWorldMatrix(),
                .mesh = renderer->mesh,
                .material = renderer->material,
                .transform = renderer->GetGameObject()->GetTransform(),
                .shader = renderer->material->GetShader(),
            };

            IResourceSet *resourceSet = renderDevice->CreateResourceSet();
            for (auto i = 0; i < materials[drawCall.material->GetId().Id].textures.size(); i++) {
                resourceSet->BindTexture(i, materials[drawCall.material->GetId().Id].textures[i]);
            }
            resourceSet->BindTexture(2, depthTexture);

            commandBuffer->BindPipeline(materials[drawCall.material->GetId().Id].pipeline);
            commandBuffer->BindVertexBuffer(drawCall.mesh->vertexBuffer);
            commandBuffer->BindIndexBuffer(drawCall.mesh->indexBuffer);
            commandBuffer->BindUniformBuffer(2, materials[drawCall.material->GetId().Id].ubo);
            commandBuffer->PushConstants(3, drawCall.transform->GetWorldMatrix());
            //commandBuffer->PushConstants(11, lights[1]->GetGameObject()->GetTransform()->Perspective(60.0f, 1.0f, 1.0f, 40.0f) * lights[1]->GetGameObject()->GetTransform()->LookAt());
            commandBuffer->PushConstants(4, lights[1]->GetGameObject()->GetTransform()->Othographic(-100.0f, 100.0f, -100.0f, 100.0f, 1.0f, 100.0f) * lights[1]->GetGameObject()->GetTransform()->LookAt());
            commandBuffer->BindResourceSet(0, *resourceSet);
            commandBuffer->Draw(drawCall.mesh->GetIndexCount());
        }

        commandBuffer->EndRenderPass(attachmentDescription2);

        renderDevice->SubmitCommandBuffer(commandBuffer);

        renderDevice->Present();

        commandBuffer->ResetAll();
    }

    void Renderer::Render() {
        // renderQueue.Sort();
        // renderDevice->Present();
    }

    void Renderer::EndFrame() {
        lightRenderer->Clear();
        //  renderPasses.clear();
    }

    Shader *Renderer::GetLitShader() const {
        return litShader;
    }
} // namespace Rendering
