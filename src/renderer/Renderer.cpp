//
// Created by zykov on 4/30/2026.
//

#include "Renderer.h"

#include <chrono>
#include <cstring>
#include <ranges>

#include "CameraData.h"
#include "gl/OGLRenderDevice.h"
#include "LightRenderer.h"
#include "RasterizedRenderPass.h"
#include "../components/light/AmbientLight.h"
#include "../components/light/DirectionalLight.h"
#include "domain/MeshPrimitives.h"
#include "gl/CommandBuffer.h"
#include "domain/buffer/BufferDesc.h"
#include "domain/struct/MaterialData.h"
#include "vulkan/VulkanRenderDevice.h"
#include "vulkan/pipeline/VulkanPipelineManager.h"
#include "../src/renderer/IRenderer.h"
#include "domain/LightRenderData.h"

namespace Rendering {
    struct CameraBuffer {
        Matrix4x4 view = Matrix4x4(1.0f);
        Matrix4x4 proj = Matrix4x4(1.0f);
        Vector4 position = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    };

    struct ModelBuffer {
        Matrix4x4 model = Matrix4x4(1.0f);
    };

    Renderer::Renderer(const RenderConfig &renderConfig, IWindow &window) : litShader(renderConfig.litShader),
                                                                            renderDevice(nullptr),
                                                                            commandBuffer(nullptr) {
        switch (renderConfig.api) {
            case RenderAPI::Vulkan: renderDevice = new VulkanRenderDevice(window);
                break;

            case RenderAPI::OpenGL: renderDevice = new OGLRenderDevice(window);
                break;

            default: std::cerr << "Render API " << renderConfig.api << "is not supported yet";
                break;
        }


        depthFormat = renderConfig.depthFormat;

        vertexLayout.bindings.push_back(VertexBinding{
            .bufferIndex = 0, .stride = sizeof(Vertex), .stepRate = VertexStepRate::PerVertex
        });
        vertexLayout.attributes.push_back(VertexAttribute{
            .location = 0, .format = VertexFormat::Float3, .offset = offsetof(Vertex, position), .bufferIndex = 0
        });
        vertexLayout.attributes.push_back(VertexAttribute{
            .location = 1, .format = VertexFormat::Float3, .offset = offsetof(Vertex, normal), .bufferIndex = 0
        });
        vertexLayout.attributes.push_back(VertexAttribute{
            .location = 2, .format = VertexFormat::Float2, .offset = offsetof(Vertex, texCoords), .bufferIndex = 0
        });

        viewport = window.GetCurrentSize();

        const ImageFormat displayImageFormat = renderDevice->GetDisplayFormat();

        IRenderPass *rasterizedRenderPass = new RasterizedRenderPass(displayImageFormat);

        renderPasses.push_back(rasterizedRenderPass);

        const std::vector attachments = {
            AttachmentDescription{
                .format = displayImageFormat,
                .type = AttachmentType::COLOR,
                .loadOp = AttachmentLoadOp::CLEAR,
                .storeOp = AttachmentStoreOp::STORE
            },
            AttachmentDescription{
                .format = depthFormat,
                .type = AttachmentType::DEPTH,
                .loadOp = AttachmentLoadOp::CLEAR,
                .storeOp = AttachmentStoreOp::DONT_CARE
            }
        };

        const auto clearColor = Color{0.75f, 0.75f, 0.75f, 1.0f};

        const auto mainRenderPass = RenderPass{
            attachments,
            viewport,
            clearColor
        };

        renderPassesDesc.push_back(mainRenderPass);

        // resource set layouts for cameras and lights set = 0 binding = 0 for camera, and binding = 1 for light

        ResourceSetLayoutDesc cameraAndLightLayout;
        cameraAndLightLayout.set_index = 0;

        ResourceBindingDesc binding_camera;
        binding_camera.binding_slot = 0;
        binding_camera.type = ResourceType::UniformBuffer;
        binding_camera.stage_flags = ShaderStageFlags::Vertex;
        binding_camera.count = 1;

        ResourceBindingDesc binding_light;
        binding_light.binding_slot = 1;
        binding_light.type = ResourceType::UniformBuffer;
        binding_light.stage_flags = ShaderStageFlags::Fragment;
        binding_light.count = 1;

        cameraAndLightLayout.bindings.push_back(binding_camera);
        cameraAndLightLayout.bindings.push_back(binding_light);

        cameraAndLightSetLayout = renderDevice->CreateResourceSetLayout(cameraAndLightLayout);

        ResourceSetLayoutDesc modelSetLayoutDesc;
        modelSetLayoutDesc.set_index = 1;

        ResourceBindingDesc binding_model;
        binding_model.binding_slot = 2;
        binding_model.type = ResourceType::UniformBuffer;
        binding_model.stage_flags = ShaderStageFlags::Vertex;
        binding_model.count = 1;

        modelSetLayoutDesc.bindings.push_back(binding_model);

        modelSetLayout = renderDevice->CreateResourceSetLayout(modelSetLayoutDesc);

        // // Setup lights
        // BufferDesc lightBuffer;
        // lightBuffer.size = sizeof(LightData);
        // lightBuffer.index = 4;
        // lightBuffer.usage = BufferUsage::Uniform;
        // lightBuffer.set = 4;
        // lightBuffer.isDynamic = true;
        //
        // lightRenderData.buffer = renderDevice->CreateBuffer(lightBuffer, nullptr);
        // lightRenderData.resourceSet = renderDevice->CreateResourceSet({.Id = 999999}, ResourceBinding{
        //                                                                   .slot = 0, .binding = 1,
        //                                                                   .type = ResourceType::UniformBuffer,
        //                                                                   .resourceId = 999997
        //                                                               });
        // renderDevice->UpdateResourceSet(lightRenderData.resourceSet, lightRenderData.buffer);

        // switch (renderConfig.api) {
        //     case RenderAPI::OpenGL: renderDevice = new OGLRenderDevice(window);
        //         break;
        //     default: std::cerr << "Render API " << renderConfig.api << "is not supported yet";
        //         break;
        // }
        //
        // commandBuffer = new OGLCommandBuffer(1024 * 1024);
        //
        // lightRenderer = new LightRenderer(*renderDevice);
        //
        // BufferDesc bufferDesc;
        // bufferDesc.size = sizeof(CameraData);
        // bufferDesc.index = 0;
        // bufferDesc.usage = BufferUsage::Uniform;
        //
        // cameraBufferId = this->renderDevice->CreateBuffer(bufferDesc, nullptr);
        //
        // renderDevice->BindUniformBuffer(cameraBufferId, 0);
        //
        // TextureCreateRequest request;
        // request.Format = TextureFormat::DEPTH;
        // request.Width = 2048;
        // request.Height = 2048;
        // request.MipLevels = 0;
        //
        // depthTexture = this->renderDevice->CreateTexture(request, nullptr);
        // depthBuffer = this->renderDevice->CreateFrameBuffer(depthTexture);
        //
        // VertexAttribute position_attribute;
        // position_attribute.format = VertexFormat::Float3;
        // position_attribute.location = 0;
        // position_attribute.offset = 0;
        // position_attribute.bufferIndex = 0;
        //
        // VertexBinding buffer_binding;
        // buffer_binding.bufferIndex = 0;
        // buffer_binding.stride = sizeof(Vertex);
        // buffer_binding.stepRate = VertexStepRate::PerVertex;
        //
        // VertexLayout vertex_layout;
        // vertex_layout.attributes.push_back(position_attribute);
        // vertex_layout.bindings.push_back(buffer_binding);
        //
        // PipelineStateDesc desc;
        // desc.depthStencilState.depthTestEnable = true;
        // desc.depthStencilState.depthWriteEnable = true;
        // desc.rasterizerState.cullEnable = true;
        // desc.rasterizerState.cullMode = CullMode::Front;
        // desc.rasterizerState.fillMode = FillMode::Solid;
        // desc.vertexLayout = vertex_layout;
        //
        // shadowPipeline = this->renderDevice->CreatePipeline(renderConfig.shadowShader->GetShader(), desc);
    }

    Renderer::~Renderer() = default;

    CameraRender *Renderer::CreateCamera(float fov, float zNear, float zFar, float aspectRatio) {
        BufferDesc cameraBuffer;
        cameraBuffer.size = sizeof(CameraBuffer);
        cameraBuffer.index = 0;
        cameraBuffer.usage = BufferUsage::Uniform;
        cameraBuffer.set = 0;
        cameraBuffer.isDynamic = true;

        CameraRenderData camera;
        camera.ViewProjectionBuffer = renderDevice->CreateBuffer(cameraBuffer, nullptr);
        camera.ResourceSet = renderDevice->CreateResourceSet(cameraAndLightSetLayout);
        renderDevice->UpdateResourceSet(camera.ResourceSet, 0, camera.ViewProjectionBuffer);

        cameras.push_back(camera);

        // Setup lights
        BufferDesc lightBuffer;
        lightBuffer.size = sizeof(LightData);
        lightBuffer.index = 2;
        lightBuffer.usage = BufferUsage::Uniform;
        lightBuffer.set = 0;
        lightBuffer.isDynamic = true;

        lightRenderData.buffer = renderDevice->CreateBuffer(lightBuffer, nullptr);
        lightRenderData.resourceSet = camera.ResourceSet;
        renderDevice->UpdateResourceSet(lightRenderData.resourceSet, 1, lightRenderData.buffer);

        const auto cameraRender = new CameraRender();
        cameraRender->handle = CameraHandle{.Id = cameras.size() - 1};

        return cameraRender;
    }

    TextureHandle Renderer::CreateTexture(void *data, TextureCreateRequest &textureCreateDesc) {
        return renderDevice->CreateTexture(textureCreateDesc, data);
    }

    MaterialHandle Renderer::CreateMaterial(const ShaderHandle shaderHandle, const MaterialDesc &materialDesc) {
        GraphicsShader shader = shaders[shaderHandle.Id];

        MaterialData data;
        data.shader = shaderHandle;

        BufferDesc bufferDesc;
        bufferDesc.size = 32;
        bufferDesc.binding = 3;
        bufferDesc.set = 2;
        bufferDesc.usage = BufferUsage::Uniform;
        bufferDesc.isDynamic = true;

        data.ubo = renderDevice->CreateBuffer(bufferDesc, nullptr);

        const ResourceSetHandle resourceSetHandle = renderDevice->CreateResourceSet(shader.resourceSetLayout);
        data.resourceSet = resourceSetHandle;

        const MaterialHandle handle{.Id = static_cast<uint32_t>(materials.size())};

        materials.push_back(data);

        BindBufferToMaterial(handle, 3, data.ubo);

        return handle;
    }

    ShaderHandle Renderer::CreateShader(const ShaderDesc &shaderDesc) {
        GraphicsShader graphicsShader;

        PipelineStateDesc pipelineStateDesc;
        pipelineStateDesc.VertexLayout = vertexLayout;
        pipelineStateDesc.Viewport = viewport;
        pipelineStateDesc.DepthFormat = depthFormat;

        graphicsShader.resourceSetLayout = renderDevice->CreateResourceSetLayout(shaderDesc.resourceSetLayouts.at(2));
        graphicsShader.pipeline = renderDevice->CreatePipeline(shaderDesc, pipelineStateDesc);

        shaders.push_back(graphicsShader);

        return ShaderHandle { .Id = shaders.size() - 1 };
    }

    LightHandle Renderer::AddLight(Light &light) {
        LightHandle handle{};

        switch (light.type) {
            case LightType::Ambient: {
                const auto ambientLight = dynamic_cast<AmbientLight *>(&light);
                lightRenderData.data.ambientLight.colorIntensity = Vector4(ambientLight->color.rgb(), ambientLight->intensity);
                LightItemData itemData;
                itemData.offset = 0;
                itemData.size = sizeof(lightRenderData.data.ambientLight);
                lights.push_back(itemData);
                handle = LightHandle{.id = lights.size() - 1};
                break;
            }
            case LightType::Directional: {
                break;
            }
            case LightType::Point: {
                break;
            }
            case LightType::Spot: {
                break;
            }
        }

        return handle;
    }

    LightHandle Renderer::AddLight(const LightType type) {
        LightHandle handle{};

        switch (type) {
            case LightType::Ambient: {
                LightItemData itemData;
                itemData.offset = 0;
                itemData.size = sizeof(lightRenderData.data.ambientLight);
                lights.push_back(itemData);
                handle = LightHandle{.id = lights.size() - 1};
                break;
            }
            case LightType::Directional: {
                LightData lightData = lightRenderData.data;

                LightItemData itemData;
                // itemData.offset = offsetof(lightData, lightRenderData.data.directionalLight);
                itemData.offset = sizeof(lightRenderData.data.ambientLight);
                itemData.size = sizeof(lightRenderData.data.directionalLight);
                lights.push_back(itemData);
                handle = LightHandle{.id = lights.size() - 1};
                break;
            }
            case LightType::Point: {
                break;
            }
            case LightType::Spot: {
                break;
            }
        }

        return handle;
    }

    void Renderer::BindTextureToMaterial(const Material &material, const uint32_t slot, const Texture &texture) {
        const MaterialData materialData = materials[material.GetId().Id];
        renderDevice->UpdateResourceSet(materialData.resourceSet, slot, texture.GetId());
    }

    void Renderer::BindBufferToMaterial(const MaterialHandle materialHandle, const uint32_t slot, const BufferHandle bufferId) {
        const MaterialData material = materials[materialHandle.Id];
        renderDevice->UpdateResourceSet(material.resourceSet, slot, bufferId);
    }

    void Renderer::UpdateMaterialData(const MaterialHandle handle, const size_t offset, const size_t size, void *data) {
        UpdateBufferData(materials[handle.Id].ubo, offset, size, data);
    }

    void Renderer::UpdateTextureFilterMode(TextureHandle &textureId, const TextureWrap textureWrap) {
        // renderDevice->UpdateTextureFilterMode(textureId, textureWrap);
    }

    void Renderer::UpdateBufferData(const BufferHandle bufferId, const size_t offset, const size_t size, void *data) {
        renderDevice->UpdateBuffer(bufferId, data, offset, size);
    }

    Mesh *Renderer::CreateMesh(const std::vector<Vertex> &vertices, const std::vector<uint16_t> &indices) {
        BufferDesc bufferDesc;
        bufferDesc.size = vertices.size() * sizeof(Vertex);
        bufferDesc.index = 0;
        bufferDesc.usage = BufferUsage::Vertex;

        const BufferHandle vertexBuffer = renderDevice->CreateBuffer(bufferDesc, vertices.data());

        bufferDesc.size = indices.size() * sizeof(uint16_t);
        bufferDesc.index = 1;
        bufferDesc.usage = BufferUsage::Index;

        const BufferHandle indexBuffer = renderDevice->CreateBuffer(bufferDesc, indices.data());

        bufferDesc.size = sizeof(ModelBuffer);
        bufferDesc.index = 1;
        bufferDesc.usage = BufferUsage::Uniform;
        bufferDesc.set = 1;
        bufferDesc.isDynamic = true;

        const BufferHandle modelBuffer = renderDevice->CreateBuffer(bufferDesc, nullptr);
        const ResourceSetHandle modelResourceSet = renderDevice->CreateResourceSet(modelSetLayout);
        renderDevice->UpdateResourceSet(modelResourceSet, 2, modelBuffer);

        const auto mesh = new Mesh(vertices, indices);
        mesh->vertexBuffer = vertexBuffer;
        mesh->indexBuffer = indexBuffer;
        mesh->modelBuffer = modelBuffer;
        mesh->modelResourceSet = modelResourceSet;
        return mesh;
    }

    void Renderer::BeginFrame(Camera &camera) {
        activeCamera = &camera;

        CameraBuffer cameraBuffer;
        cameraBuffer.view = activeCamera->GetTransform()->LookAt();
        cameraBuffer.proj = activeCamera->GetProjection();
        cameraBuffer.position = Vector4(activeCamera->GetTransform()->position, 1.0f);

        renderDevice->UpdateBuffer(cameras[activeCamera->GetHandle().Id].ViewProjectionBuffer, &cameraBuffer, 0,
                                   sizeof(CameraBuffer));
    }

    void Renderer::Submit(Scene &scene) {
        const std::vector<MeshRenderer *> &renderers = scene.GetComponents<MeshRenderer>();
        const std::vector<Light *> &lights = scene.GetComponents<Light>();

        constexpr Rect viewport{0.0f, 0.0f, 2560.0f, 1440.0f};
        const Color clearColor{0.1f, 0.1f, 0.1f, 1.0f};

        AttachmentDescription attachmentDescription1{};
        attachmentDescription1.loadOp = AttachmentLoadOp::CLEAR;
        attachmentDescription1.storeOp = AttachmentStoreOp::STORE;
        attachmentDescription1.type = AttachmentType::DEPTH;

        AttachmentDescription attachmentDescription2{};
        attachmentDescription2.loadOp = AttachmentLoadOp::CLEAR;
        attachmentDescription2.storeOp = AttachmentStoreOp::DONT_CARE;
        attachmentDescription2.type = AttachmentType::COLOR;

        DrawCall drawCalls[renderers.size()];
        int i = 0;
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
            drawCalls[i] = drawCall;
            i++;
        }

        // lights
        lightRenderer->LoadLights(scene);
        lightRenderer->EnableLights();

        // shadow pass

        commandBuffer->BeginRenderPass(attachmentDescription1, depthBuffer.Id, clearColor);

        for (const auto &drawCall: drawCalls) {
            commandBuffer->BindPipeline(shadowPipeline);
            commandBuffer->BindVertexBuffer(drawCall.mesh->vertexBuffer);
            commandBuffer->BindIndexBuffer(drawCall.mesh->indexBuffer);
            commandBuffer->PushConstants(3, drawCall.transform->GetWorldMatrix());
            //  commandBuffer->PushConstants(11, lights[1]->GetGameObject()->GetTransform()->Perspective(60.0f, 1.0f, 1.0f, 40.0f) * lights[1]->GetGameObject()->GetTransform()->LookAt());
            commandBuffer->PushConstants(
                4, lights[1]->GetGameObject()->GetTransform()->Othographic(
                       -100.0f, 100.0f, -100.0f, 100.0f, 1.0f,
                       100.0f) * lights[1]->GetGameObject()->GetTransform()->LookAt());
            commandBuffer->Draw(drawCall.mesh->GetIndexCount());
        }

        commandBuffer->EndRenderPass(attachmentDescription1);

        //  renderDevice->SubmitCommandBuffer(commandBuffer);

        //Engine::SaveDepthTextureToFile(1, 2048, 2048, "depth.png");
        //
        // return;
        // main pass

        // -1 is window
        commandBuffer->BeginRenderPass(attachmentDescription2, -1, viewport.width, viewport.height, clearColor);

        const CameraData cameraData{
            .view = activeCamera->GetTransform()->LookAt(),
            .projection = activeCamera->GetProjection(),
            .position = Vector4(activeCamera->GetTransform()->position, 1),
        };

        commandBuffer->UpdateBuffer(cameraBufferId, &cameraData, 0, sizeof(CameraData));

        for (const auto &drawCall: drawCalls) {
            //  IResourceSet *resourceSet = renderDevice->CreateResourceSet();
            //  for (auto i = 0; i < materials[drawCall.material->GetId().Id].textures.size(); i++) {
            //      resourceSet->BindTexture(i, materials[drawCall.material->GetId().Id].textures[i]);
            //  }
            //  resourceSet->BindTexture(2, depthTexture);
            //
            //  commandBuffer->BindPipeline(materials[drawCall.material->GetId().Id].pipeline);
            //  commandBuffer->BindVertexBuffer(drawCall.mesh->vertexBuffer);
            //  commandBuffer->BindIndexBuffer(drawCall.mesh->indexBuffer);
            //  commandBuffer->BindUniformBuffer(2, materials[drawCall.material->GetId().Id].ubo);
            //  commandBuffer->PushConstants(3, drawCall.transform->GetWorldMatrix());
            //  //commandBuffer->PushConstants(11, lights[1]->GetGameObject()->GetTransform()->Perspective(60.0f, 1.0f, 1.0f, 40.0f) * lights[1]->GetGameObject()->GetTransform()->LookAt());
            //  commandBuffer->PushConstants(
            //      4, lights[1]->GetGameObject()->GetTransform()->Othographic(
            //             -100.0f, 100.0f, -100.0f, 100.0f, 1.0f,
            //             100.0f) * lights[1]->GetGameObject()->GetTransform()->LookAt());
            // // commandBuffer->BindResourceSet(0, *resourceSet);
            //  commandBuffer->Draw(drawCall.mesh->GetIndexCount());
        }

        commandBuffer->EndRenderPass(attachmentDescription2);

        renderDevice->SubmitCommandBuffer(commandBuffer);

        renderDevice->Present();

        commandBuffer->ResetAll();
    }

    void Renderer::Render(Scene &scene) {
        const auto cmd = renderDevice->AllocateCommandBuffer();

        cmd->Begin();

        const std::vector<DrawCall> drawCalls = scene.GetDrawCalls();
        const std::vector<Light *> lights = scene.GetLights();
        std::unordered_map<Material *, DrawCall> _materials;

        for (const auto drawCall: drawCalls) {
            const auto modelBufferHandle = drawCall._mesh->modelBuffer;
            ModelBuffer modelBuffer;
            modelBuffer.model = drawCall.transform->GetWorldMatrix();
            renderDevice->UpdateBuffer(modelBufferHandle, &modelBuffer, 0, sizeof(ModelBuffer));
            _materials.emplace(drawCall.material, drawCall);
        }

        auto keys_view = std::views::keys(_materials);
        std::vector keys(keys_view.begin(), keys_view.end());

        for (const auto material: keys) {
            if (material->dirtyFrames > 0) {
                cmd->UpdateUniformBuffer(materials[material->GetId().Id].ubo, material->GetBufferData().data(), 0,
                                         material->GetBufferData().size());
                material->dirtyFrames--;
            }
        }

        for (const auto light: lights) {
            const uint32_t offset = this->lights[light->GetHandle().id].offset;
            const uint32_t size = this->lights[light->GetHandle().id].size;

            switch (light->type) {
                case LightType::Ambient: {
                    const auto lightData = dynamic_cast<AmbientLight *>(light);
                    const auto temp = Vector4(lightData->color.rgb(), lightData->intensity);
                    lightRenderData.data.ambientLight.colorIntensity = temp;
                    renderDevice->UpdateBuffer(lightRenderData.buffer,
                                               &lightRenderData.data.ambientLight.colorIntensity, offset, size);
                    break;
                }
                case LightType::Directional: {
                    const auto lightData = dynamic_cast<DirectionalLight *>(light);
                    const auto temp = Vector4(lightData->color.rgb(), lightData->intensity);
                    lightRenderData.data.directionalLight.colorIntensity = temp;
                    lightRenderData.data.directionalLight.direction = Vector4(
                        light->GetGameObject()->GetTransform()->Forward(), 1);
                    lightRenderData.data.directionalLight.space = light->GetGameObject()->GetTransform()->
                            GetWorldMatrix();
                    renderDevice->UpdateBuffer(lightRenderData.buffer, &lightRenderData.data.directionalLight, offset,
                                               size);
                    break;
                }
                default: ;
            }
        }
        //

        cmd->BeginRenderPass(renderPassesDesc[0]);
        cmd->BindResourceSet(cameras[activeCamera->GetHandle().Id].ResourceSet, PipelineHandle{.Id = 0}, 0);
        const Material* lastMaterial = nullptr;
        for (const auto drawCall: drawCalls) {
            // material properties
            if (lastMaterial != drawCall.material) {
                lastMaterial = drawCall.material;
                cmd->BindPipeline(shaders[materials[drawCall.material->GetMaterialHandle().Id].shader.Id].pipeline);
                cmd->BindResourceSet(materials[drawCall.material->GetMaterialHandle().Id].resourceSet,
                                 materials[drawCall.material->GetMaterialHandle().Id].pipeline, 2);
            }

            // mesh properties
            cmd->BindVertexBuffer(drawCall._mesh->vertexBuffer);
            cmd->BindIndexBuffer(drawCall._mesh->indexBuffer);
            cmd->BindResourceSet(drawCall._mesh->modelResourceSet,
                                 materials[drawCall.material->GetMaterialHandle().Id].pipeline, 1);
            cmd->DrawIndexed(drawCall._mesh->GetIndexCount());
        }
        cmd->EndRenderPass();
        cmd->End();

        renderDevice->SubmitCommandBuffer(cmd);

        delete cmd;
    }

    void Renderer::Render(const std::vector<DrawCall> &drawCalls) {
        const auto cmd = renderDevice->AllocateCommandBuffer();

        for (const auto drawCall: drawCalls) {
            const auto modelBufferHandle = drawCall._mesh->modelBuffer;

            ModelBuffer modelBuffer;
            modelBuffer.model = drawCall.transform->GetWorldMatrix();

            renderDevice->UpdateBuffer(modelBufferHandle, &modelBuffer, 0, sizeof(ModelBuffer));
        }

        cmd->Begin();
        cmd->BeginRenderPass(renderPassesDesc[0]);
        cmd->BindResourceSet(cameras[activeCamera->GetHandle().Id].ResourceSet, PipelineHandle{.Id = 0}, 0);
        cmd->BindResourceSet(lightRenderData.resourceSet, PipelineHandle{.Id = 0}, 3);
        for (const auto drawCall: drawCalls) {
            cmd->BindPipeline(materials[drawCall.material->GetMaterialHandle().Id].pipeline);
            cmd->BindVertexBuffer(drawCall._mesh->vertexBuffer);
            cmd->BindIndexBuffer(drawCall._mesh->indexBuffer);
            cmd->DrawIndexed(drawCall._mesh->GetIndexCount());
        }
        cmd->EndRenderPass();
        cmd->End();

        renderDevice->SubmitCommandBuffer(cmd);
        delete cmd;
    }

    void Renderer::EndFrame() {
        //lightRenderer->Clear();
    }

    Shader *Renderer::GetLitShader() const {
        return litShader;
    }
} // namespace Rendering
