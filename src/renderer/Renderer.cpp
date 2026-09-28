//
// Created by zykov on 4/30/2026.
//

#include "Renderer.h"

#include <chrono>
#include <cstring>
#include <ranges>

#include "CameraData.h"
#include "gl/OGLRenderDevice.h"
#include "RasterizedRenderPass.h"
#include "../components/light/AmbientLight.h"
#include "../components/light/DirectionalLight.h"
#include "../components/light/PointLight.h"
#include "../components/light/SpotLight.h"
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
                                                                            depthShader(nullptr),
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

        const TextureFormat displayImageFormat = renderDevice->GetDisplayFormat();

        IRenderPass *rasterizedRenderPass = new RasterizedRenderPass(displayImageFormat);

        renderPasses.push_back(rasterizedRenderPass);

        TextureCreateRequest request;
        request.Format = TextureFormat::DEPTH;
        request.Width = 2160;
        request.Height = 1440;
        request.MipLevels = 0;

        depthTexture = renderDevice->CreateTexture(request, nullptr);

        const std::array attachments = {
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

        const std::array<AttachmentDescription, 2> shadowAttachments = {
            AttachmentDescription{
                .format = depthFormat,
                .type = AttachmentType::DEPTH,
                .loadOp = AttachmentLoadOp::CLEAR,
                .storeOp = AttachmentStoreOp::STORE,
                .renderTarget = depthTexture
            }
        };

        const auto clearColor = Color{0.75f, 0.75f, 0.75f, 1.0f};

        viewport = window.GetCurrentSize();

        const auto mainRenderPass = RenderPass{
            attachments[0],
            attachments[1],
            viewport,
            clearColor
        };

        const auto shadowRenderPass = RenderPass{
            {},
            shadowAttachments[0],
            viewport,
            clearColor
        };

        renderPassesDesc.push_back(mainRenderPass);
        renderPassesDesc.push_back(shadowRenderPass);

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

        ResourceBindingDesc binding_shadow;
        binding_shadow.binding_slot = 7;
        binding_shadow.type = ResourceType::UniformBuffer;
        binding_shadow.stage_flags = ShaderStageFlags::Vertex;
        binding_shadow.count = 1;

        cameraAndLightLayout.bindings.push_back(binding_camera);
        cameraAndLightLayout.bindings.push_back(binding_light);
        // cameraAndLightLayout.bindings.push_back(binding_shadow);

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

        std::vector<ResourceSetLayoutHandle> resourceSetLayouts;
        resourceSetLayouts.push_back(cameraAndLightSetLayout);
        resourceSetLayouts.push_back(modelSetLayout);

        mainPipelineLayout = renderDevice->CreatePipelineLayout(resourceSetLayouts);
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

        // Setup shadows

        // Map
        BufferDesc lightSpaceBuffer;
        lightSpaceBuffer.size = sizeof(CameraBuffer);
        lightSpaceBuffer.index = 0;
        lightSpaceBuffer.usage = BufferUsage::Uniform;
        lightSpaceBuffer.set = 0;
        lightSpaceBuffer.isDynamic = true;

        lightSpace.ViewProjectionBuffer = renderDevice->CreateBuffer(cameraBuffer, nullptr);
        lightSpace.ResourceSet = renderDevice->CreateResourceSet(cameraAndLightSetLayout);
        renderDevice->UpdateResourceSet(lightSpace.ResourceSet, 0, lightSpace.ViewProjectionBuffer);

        // Light space

        BufferDesc shadowsBuffer;
        shadowsBuffer.size = sizeof(Matrix4x4);
        shadowsBuffer.index = 7;
        shadowsBuffer.usage = BufferUsage::Uniform;
        shadowsBuffer.set = 0;
        shadowsBuffer.isDynamic = true;

        // shadowRenderData.buffer = renderDevice->CreateBuffer(shadowsBuffer, nullptr);
        // shadowRenderData.resourceSet = camera.ResourceSet;
        // renderDevice->UpdateResourceSet(shadowRenderData.resourceSet, 7, shadowRenderData.buffer);

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
        BindTextureToMaterial(handle, 6, depthTexture);

        return handle;
    }

    ShaderHandle Renderer::CreateShader(const ShaderDesc &shaderDesc) {
        GraphicsShader graphicsShader;

        const CullMode cullMode = shaderDesc.CullMode;
        const bool isDepthOnly = shaderDesc.IsDepthOnly;

        auto colorFormat = TextureFormat::UNDEFINED;

        if (!isDepthOnly) {
            colorFormat = renderDevice->GetDisplayFormat();
        }

        PipelineStateDesc pipelineStateDesc;
        pipelineStateDesc.VertexLayout = vertexLayout;
        pipelineStateDesc.Viewport = viewport;
        pipelineStateDesc.DepthFormat = depthFormat;
        pipelineStateDesc.ColorFormat = colorFormat;
        pipelineStateDesc.CullMode = cullMode;

        if (shaderDesc.resourceSetLayouts.size() >= 3) {
            graphicsShader.resourceSetLayout = renderDevice->CreateResourceSetLayout(shaderDesc.resourceSetLayouts.at(2));
        }
        graphicsShader.pipeline = renderDevice->CreatePipeline(shaderDesc, pipelineStateDesc);

        shaders.push_back(graphicsShader);

        return ShaderHandle{.Id = shaders.size() - 1};
    }

    LightHandle Renderer::CreateLight(const LightType type) {
        LightHandle handle{};

        switch (type) {
            case LightType::Ambient: {
                LightItemData itemData;
                itemData.index = 0;
                itemData.type = type;
                lights.push_back(itemData);
                handle = LightHandle{.id = lights.size() - 1 };
                break;
            }
            case LightType::Directional: {
                LightItemData itemData;
                itemData.index = 0;
                itemData.type = type;
                lights.push_back(itemData);
                handle = LightHandle{.id = lights.size() - 1 };
                break;
            }
            case LightType::Point: {
                LightItemData itemData;
                itemData.index = ++lastPointLightIndex;
                itemData.type = type;
                lights.push_back(itemData);
                handle = LightHandle{.id = lights.size() - 1 };
                break;
            }
            case LightType::Spot: {
                LightItemData itemData;
                itemData.index = ++lastSpotLightIndex;
                itemData.type = type;
                lights.push_back(itemData);
                handle = LightHandle{.id = lights.size() - 1 };
                break;
            }
        }

        return handle;
    }

    void Renderer::BindTextureToMaterial(const MaterialHandle material, const uint32_t slot, const TextureHandle texture) const {
        const MaterialData materialData = materials[material.Id];
        renderDevice->UpdateResourceSet(materialData.resourceSet, slot, texture);
    }

    void Renderer::BindTextureToMaterial(const Material &material, const uint32_t slot, const Texture &texture) {
        const MaterialData materialData = materials[material.GetId().Id];
        renderDevice->UpdateResourceSet(materialData.resourceSet, slot, texture.GetId());
    }

    void Renderer::BindBufferToMaterial(const MaterialHandle materialHandle, const uint32_t slot,
                                        const BufferHandle bufferId) {
        const MaterialData material = materials[materialHandle.Id];
        renderDevice->UpdateResourceSet(material.resourceSet, slot, bufferId);
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
            const uint32_t index = this->lights[light->GetHandle().id].index;

            switch (light->lightType) {
                case LightType::Ambient: {
                    const auto lightData = dynamic_cast<AmbientLight *>(light);

                    const auto colorIntensity = Vector4(lightData->color.rgb(), lightData->intensity);

                    lightRenderData.data.ambientLight.colorIntensity = colorIntensity;
                    break;
                }
                case LightType::Directional: {
                    const auto lightData = dynamic_cast<DirectionalLight *>(light);

                    const auto colorIntensity = Vector4(lightData->color.rgb(), lightData->intensity);
                    const auto direction = Vector4(light->GetGameObject()->GetTransform()->Forward(), 1);
                    lightRenderData.data.directionalLight.colorIntensity = colorIntensity;
                    lightRenderData.data.directionalLight.direction = direction;
                    break;
                }
                case LightType::Point: {
                    const auto lightData = dynamic_cast<PointLight *>(light);

                    const LightHandle lightHandle = lightData->GetHandle();
                    const uint32_t lightHandleId = lightHandle.id - 2;
                    const auto color = lightData->color.rgb();
                    const auto intensity = lightData->intensity;
                    const auto colorIntensity = Vector4(color, intensity);
                    const auto position = light->GetGameObject()->GetTransform()->position;
                    const auto range = lightData->GetRange();
                    const auto positionRange = Vector4(position, range);

                    lightRenderData.data.pointLights[index].colorIntensity = colorIntensity;
                    lightRenderData.data.pointLights[index].positionRange = positionRange;
                    break;
                }
                case LightType::Spot: {
                    const auto lightData = dynamic_cast<SpotLight *>(light);

                    const LightHandle lightHandle = lightData->GetHandle();
                    const uint32_t lightHandleId = lightHandle.id - 2;
                    const auto color = lightData->color.rgb();
                    const auto intensity = lightData->intensity;
                    const auto colorIntensity = Vector4(color, intensity);
                    const auto direction = Vector4(light->GetGameObject()->GetTransform()->Forward(), 1);
                    const auto position = light->GetGameObject()->GetTransform()->position;
                    const auto range = lightData->GetRange();
                    const auto positionRange = Vector4(position, range);
                    const auto innerAngle = lightData->GetInnerAngle();
                    const auto outerAngle = lightData->GetOuterAngle();
                    const auto coneAngle = Vector4(innerAngle, outerAngle, 1.0f, 1.0f);

                    lightRenderData.data.spotLights[index].colorIntensity = colorIntensity;
                    lightRenderData.data.spotLights[index].direction = direction;
                    lightRenderData.data.spotLights[index].coneAngle = coneAngle;
                    lightRenderData.data.spotLights[index].positionRange = positionRange;
                    break;
                }
            }
        }

        // Light persp

        CameraBuffer cameraBuffer;
        cameraBuffer.view = lights[1]->GetGameObject()->GetTransform()->LookAt();
        cameraBuffer.proj = lights[1]->GetGameObject()->GetTransform()->Perspective(90.0f, 2160.0f / 1440.0f, 0.1f, 100.0f);
        cameraBuffer.position = Vector4(lights[1]->GetGameObject()->GetTransform()->position, 1.0f);

        Matrix4x4 pv = cameraBuffer.proj * cameraBuffer.view;
        lightRenderData.data.directionalLight.space = pv;

        renderDevice->UpdateBuffer(lightRenderData.buffer, &lightRenderData.data, 0, sizeof(lightRenderData.data));
        renderDevice->UpdateBuffer(lightSpace.ViewProjectionBuffer, &cameraBuffer, 0, sizeof(CameraBuffer));

        cmd->BeginRenderPass(renderPassesDesc[1]);
        cmd->BindPipeline(shaders[depthShader->GetHandle().Id].pipeline);
        cmd->BindResourceSet(lightSpace.ResourceSet, shaders[depthShader->GetHandle().Id].pipeline, 0);

        for (const auto drawCall: drawCalls) {
            cmd->BindVertexBuffer(drawCall._mesh->vertexBuffer);
            cmd->BindIndexBuffer(drawCall._mesh->indexBuffer);
            cmd->BindResourceSet(drawCall._mesh->modelResourceSet, shaders[depthShader->GetHandle().Id].pipeline, 1);
            cmd->DrawIndexed(drawCall._mesh->GetIndexCount());
        }

        cmd->EndRenderPass();

        // Camera persp

        cameraBuffer.view = activeCamera->GetTransform()->LookAt();
        cameraBuffer.proj = activeCamera->GetProjection();
        cameraBuffer.position = Vector4(activeCamera->GetTransform()->position, 1.0f);

        renderDevice->UpdateBuffer(cameras[activeCamera->GetHandle().Id].ViewProjectionBuffer, &cameraBuffer, 0,
                                   sizeof(CameraBuffer));

        //

        cmd->BeginRenderPass(renderPassesDesc[0]);
        cmd->BindResourceSet(cameras[activeCamera->GetHandle().Id].ResourceSet, mainPipelineLayout, 0);

        const Material *lastMaterial = nullptr;
        for (const auto drawCall: drawCalls) {
            // material properties
            if (lastMaterial != drawCall.material) {
                lastMaterial = drawCall.material;
                cmd->BindPipeline(shaders[materials[lastMaterial->GetMaterialHandle().Id].shader.Id].pipeline);
                cmd->BindResourceSet(materials[lastMaterial->GetMaterialHandle().Id].resourceSet,
                                     materials[lastMaterial->GetMaterialHandle().Id].pipeline, 2);
            }

            // mesh properties
            cmd->BindVertexBuffer(drawCall._mesh->vertexBuffer);
            cmd->BindIndexBuffer(drawCall._mesh->indexBuffer);
            cmd->BindResourceSet(drawCall._mesh->modelResourceSet,
                                 mainPipelineLayout, 1);
            cmd->DrawIndexed(drawCall._mesh->GetIndexCount());
        }

        cmd->EndRenderPass();
        cmd->End();

        renderDevice->SubmitCommandBuffer(cmd);

        delete cmd;
    }

    void Renderer::EndFrame() {

    }

    void Renderer::SetDepthShader(Shader &shader) {
        depthShader = &shader;
    }
} // namespace Rendering
