//
// Created by zykov on 4/30/2026.
//

#include "Renderer.h"

#include <chrono>
#include <cstring>
#include <iostream>
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
    struct CameraBoxBuffer {
        Matrix4x4 proj = Matrix4x4::Identity();
        Matrix4x4 view[6];
    };

    struct CameraBuffer {
        Matrix4x4 view = Matrix4x4::Identity();
        Matrix4x4 proj = Matrix4x4::Identity();
        Vector4 position = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    };

    struct ModelBuffer {
        Matrix4x4 model = Matrix4x4::Identity();
    };

    Renderer::Renderer(const RenderConfig &renderConfig, IWindow &window) : depthShader(nullptr),
                                                                            renderDevice(nullptr) {
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
        vertexLayout.attributes.push_back(VertexAttribute{
            .location = 3, .format = VertexFormat::Float3, .offset = offsetof(Vertex, tangent), .bufferIndex = 0
        });
        vertexLayout.attributes.push_back(VertexAttribute{
            .location = 4, .format = VertexFormat::Float3, .offset = offsetof(Vertex, bitangent), .bufferIndex = 0
        });
        vertexLayout.bindings.push_back(VertexBinding{
            .bufferIndex = 1, .stride = sizeof(int), .stepRate = VertexStepRate::PerInstance
        });
        vertexLayout.attributes.push_back(VertexAttribute{
            .location = 5, .format = VertexFormat::Float, .offset = 0, .bufferIndex = 1
        });

        renderDevice->CreateVao(vertexLayout);

        viewport = window.GetCurrentSize();

        const TextureFormat displayImageFormat = renderDevice->GetDisplayFormat();

        IRenderPass *rasterizedRenderPass = new RasterizedRenderPass(displayImageFormat);

        renderPasses.push_back(rasterizedRenderPass);

        TextureCreateRequest shadowTextureRequest;
        shadowTextureRequest.Format = TextureFormat::DEPTH;
        shadowTextureRequest.Width = 2160;
        shadowTextureRequest.Height = 1440;
        shadowTextureRequest.MipLevels = 0;

        shadowTexture = renderDevice->CreateTexture(shadowTextureRequest, nullptr);

        const auto clearColor = Color{0.75f, 0.75f, 0.75f, 1.0f};

        const std::array attachments = {
            AttachmentDescription{
                .type = AttachmentType::COLOR,
                .loadOp = AttachmentLoadOp::CLEAR,
                .storeOp = AttachmentStoreOp::DONT_CARE,
                .clearColor = clearColor
            },
            AttachmentDescription{
                .type = AttachmentType::DEPTH,
                .loadOp = AttachmentLoadOp::CLEAR,
                .storeOp = AttachmentStoreOp::DONT_CARE
            }
        };

        const std::array skyboxAttachments = {
            AttachmentDescription{
                .type = AttachmentType::COLOR,
                .loadOp = AttachmentLoadOp::LOAD,
                .storeOp = AttachmentStoreOp::STORE,
                .clearColor = clearColor,
            },
            AttachmentDescription{
                .type = AttachmentType::DEPTH,
                .loadOp = AttachmentLoadOp::LOAD,
                .storeOp = AttachmentStoreOp::STORE
            }
        };

        TextureCreateRequest envCubeMapRequest;
        envCubeMapRequest.Width = 32;
        envCubeMapRequest.Height = 32;
        envCubeMapRequest.Type = TextureType::Cube;
        envCubeMapRequest.Filter = TextureFilter::Bilinear;
        envCubeMapRequest.Wrap = TextureWrap::Clamp;
        envCubeMapRequest.Format = TextureFormat::RGBA;
        irradianceCubeMap = Renderer::CreateCubeMap(envCubeMapRequest, {});

        const std::array envAttachments = {
            AttachmentDescription{
                .type = AttachmentType::COLOR,
                .loadOp = AttachmentLoadOp::CLEAR,
                .storeOp = AttachmentStoreOp::DONT_CARE,
                .clearColor = clearColor,
                .renderCubeMap = irradianceCubeMap
            },
            AttachmentDescription{
                .type = AttachmentType::DEPTH,
                .loadOp = AttachmentLoadOp::CLEAR,
                .storeOp = AttachmentStoreOp::DONT_CARE
            }
        };

        TextureCreateRequest brdfTextureRequest;
        brdfTextureRequest.Width = 512;
        brdfTextureRequest.Height = 512;
        brdfTextureRequest.MipLevels = 0;
        brdfTextureRequest.Type = TextureType::Texture2D;
        brdfTextureRequest.Filter = TextureFilter::Bilinear;
        brdfTextureRequest.Wrap = TextureWrap::Clamp;
        brdfTextureRequest.Format = TextureFormat::RGBA;
        brdfTexture = renderDevice->CreateTexture(brdfTextureRequest, nullptr);

        const std::array brdfAttachments = {
            AttachmentDescription{
                .type = AttachmentType::COLOR,
                .loadOp = AttachmentLoadOp::CLEAR,
                .storeOp = AttachmentStoreOp::DONT_CARE,
                .clearColor = clearColor,
                .renderTarget = brdfTexture
            },
            AttachmentDescription{
                .type = AttachmentType::DEPTH,
                .loadOp = AttachmentLoadOp::CLEAR,
                .storeOp = AttachmentStoreOp::DONT_CARE
            }
        };

        const std::array<AttachmentDescription, 2> shadowAttachments = {
            AttachmentDescription{
                .type = AttachmentType::DEPTH,
                .loadOp = AttachmentLoadOp::CLEAR,
                .storeOp = AttachmentStoreOp::STORE,
                .renderTarget = shadowTexture
            }
        };

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
            // {0, 0, 2048, 2048},
            viewport,
            clearColor
        };

        const auto skyboxRenderPass = RenderPass{
            skyboxAttachments[0],
            skyboxAttachments[1],
            viewport,
            clearColor
        };

        const auto envRenderPass = RenderPass{
            envAttachments[0],
            envAttachments[1],
            {0, 0, 32, 32},
            clearColor
        };

        renderPassesDesc.push_back(mainRenderPass);
        renderPassesDesc.push_back(shadowRenderPass);
        renderPassesDesc.push_back(skyboxRenderPass);
        renderPassesDesc.push_back(envRenderPass);

        TextureCreateRequest prefilterCubeMapRequest;
        prefilterCubeMapRequest.Width = 128;
        prefilterCubeMapRequest.Height = 128;
        prefilterCubeMapRequest.MipLevels = 4;
        prefilterCubeMapRequest.useMipMaps = true;
        prefilterCubeMapRequest.Type = TextureType::Cube;
        prefilterCubeMapRequest.Filter = TextureFilter::Trilinear;
        prefilterCubeMapRequest.Wrap = TextureWrap::Clamp;
        prefilterCubeMapRequest.Format = TextureFormat::RGBA;
        prefilterCubeMap = Renderer::CreateCubeMap(prefilterCubeMapRequest, {});

        const std::array prefilterAttachments = {
            AttachmentDescription{
                .type = AttachmentType::COLOR,
                .loadOp = AttachmentLoadOp::CLEAR,
                .storeOp = AttachmentStoreOp::DONT_CARE,
                .clearColor = clearColor,
                .renderCubeMap = prefilterCubeMap
            },
            AttachmentDescription{
                .type = AttachmentType::DEPTH,
                .loadOp = AttachmentLoadOp::CLEAR,
                .storeOp = AttachmentStoreOp::DONT_CARE
            }
        };

        unsigned int maxMipLevels = 4;
        for (int mip = 0; mip < maxMipLevels; mip++) {
            unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
            unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));

            const auto prefilterRenderPass = RenderPass{
                prefilterAttachments[0],
                prefilterAttachments[1],
                {0, 0, static_cast<float>(mipWidth) , static_cast<float>(mipHeight) },
                clearColor,
                static_cast<uint32_t>(mip)
            };

            renderPassesDesc.push_back(prefilterRenderPass);
        }

        const auto brdfRenderPass = RenderPass{
            brdfAttachments[0],
            brdfAttachments[1],
            {0, 0, 512 , 512 },
            clearColor
        };

        renderPassesDesc.push_back(brdfRenderPass);

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
        binding_model.type = ResourceType::StorageBuffer;
        binding_model.stage_flags = ShaderStageFlags::Vertex;
        binding_model.count = 1;

        modelSetLayoutDesc.bindings.push_back(binding_model);

        modelSetLayout = renderDevice->CreateResourceSetLayout(modelSetLayoutDesc);

        std::vector<ResourceSetLayoutHandle> resourceSetLayouts;
        resourceSetLayouts.push_back(cameraAndLightSetLayout);
        resourceSetLayouts.push_back(modelSetLayout);

        mainPipelineLayout = renderDevice->CreatePipelineLayout(resourceSetLayouts);

        ResourceSetLayoutDesc cameraBoxLayoutDesc;
        cameraBoxLayoutDesc.set_index = 0;

        ResourceBindingDesc bindingCameraBox;
        bindingCameraBox.binding_slot = 0;
        bindingCameraBox.type = ResourceType::UniformBuffer;
        bindingCameraBox.stage_flags = ShaderStageFlags::Vertex;
        bindingCameraBox.count = 1;

        cameraBoxLayoutDesc.bindings.push_back(bindingCameraBox);

        cameraBoxLayout = renderDevice->CreateResourceSetLayout(cameraBoxLayoutDesc);
        cameraBoxResourceSet = renderDevice->CreateResourceSet(cameraBoxLayout);

        BufferDesc cameraBufferDesc;
        cameraBufferDesc.size = sizeof(CameraBoxBuffer);
        cameraBufferDesc.usage = BufferUsage::Uniform;
        cameraBufferDesc.isDynamic = true;
        cameraBoxBuffer = renderDevice->CreateBuffer(cameraBufferDesc);
        renderDevice->UpdateResourceSet(cameraBoxResourceSet, 0, cameraBoxBuffer);

        // Setup model
        BufferDesc modelBufferDesc;
        modelBufferDesc.size = sizeof(ModelBuffer) * 2000;
        modelBufferDesc.usage = BufferUsage::Storage;
        modelBufferDesc.isDynamic = true;

        modelBuffer = renderDevice->CreateBuffer(modelBufferDesc);
        modelResourceSet = renderDevice->CreateResourceSet(modelSetLayout);
        renderDevice->UpdateResourceSet(modelResourceSet, 2, modelBuffer);

        // Setup static meshes

        staticMeshes.currentVertexOffset = 0;
        staticMeshes.currentIndexOffset = 0;

        BufferDesc staticVertexBufferDesc;
        staticVertexBufferDesc.size = sizeof(Vertex) * 1000000;
        staticVertexBufferDesc.usage = BufferUsage::Vertex;
        staticVertexBufferDesc.isNewApproach = true;
        staticMeshes.vertexBuffer = renderDevice->CreateBuffer(staticVertexBufferDesc);

        BufferDesc staticIndexBufferDesc;
        staticIndexBufferDesc.size = sizeof(uint16_t) * 1000000;
        staticIndexBufferDesc.usage = BufferUsage::Index;
        staticIndexBufferDesc.isNewApproach = true;
        staticMeshes.indexBuffer = renderDevice->CreateBuffer(staticIndexBufferDesc);

        std::vector<int> customInstanceIDs(10000);
        for(int i = 0; i < 10000; i++) {
            customInstanceIDs[i] = i;
        }

        BufferDesc customInstanceDesc;
        customInstanceDesc.size = customInstanceIDs.size() * sizeof(int);
        customInstanceDesc.usage = BufferUsage::Vertex;
        customInstanceDesc.isNewApproach = true;
        instanceBuffer = renderDevice->CreateBuffer(customInstanceDesc);
       // renderDevice->UpdateBuffer(instanceBuffer, customInstanceIDs.data(), 0, customInstanceIDs.size() * sizeof(int));

        uploadCommandBuffer = renderDevice->AllocateCommandBuffer(CommandBufferType::Transfer);
        uploadCommandBuffer->Begin();
    }

    Renderer::~Renderer() = default;

    CameraRender *Renderer::CreateCamera(float fov, float zNear, float zFar, float aspectRatio) {
        BufferDesc cameraBuffer;
        cameraBuffer.size = sizeof(CameraBuffer);
        cameraBuffer.usage = BufferUsage::Uniform;
        cameraBuffer.isDynamic = true;

        CameraRenderData camera;
        camera.ViewProjectionBuffer = renderDevice->CreateBuffer(cameraBuffer);
        camera.ResourceSet = renderDevice->CreateResourceSet(cameraAndLightSetLayout);
        renderDevice->UpdateResourceSet(camera.ResourceSet, 0, camera.ViewProjectionBuffer);

        cameras.push_back(camera);

        // Setup lights
        BufferDesc lightBuffer;
        lightBuffer.size = sizeof(LightData);
        lightBuffer.usage = BufferUsage::Uniform;
        lightBuffer.isDynamic = true;

        lightRenderData.buffer = renderDevice->CreateBuffer(lightBuffer);
        lightRenderData.resourceSet = camera.ResourceSet;
        renderDevice->UpdateResourceSet(lightRenderData.resourceSet, 1, lightRenderData.buffer);

        lightSpace.ViewProjectionBuffer = renderDevice->CreateBuffer(cameraBuffer);
        lightSpace.ResourceSet = renderDevice->CreateResourceSet(cameraAndLightSetLayout);
        renderDevice->UpdateResourceSet(lightSpace.ResourceSet, 0, lightSpace.ViewProjectionBuffer);

        const auto cameraRender = new CameraRender();
        cameraRender->handle = CameraHandle{.Id = cameras.size() - 1};

        return cameraRender;
    }

    TextureHandle Renderer::CreateTexture(void *data, TextureCreateRequest &textureCreateDesc) {
        return renderDevice->CreateTexture(textureCreateDesc, data);
    }

    CubeMapHandle Renderer::CreateCubeMap(const TextureCreateRequest &desc, const std::vector<void *> &datas) {
        return renderDevice->CreateCubeMap(desc, datas);
    }

    MaterialHandle Renderer::CreateMaterial(const ShaderHandle shaderHandle, const MaterialDesc &materialDesc) {
        GraphicsShader shader = shaders[shaderHandle.Id];

        MaterialData data;
        data.shader = shaderHandle;

        if (materialDesc.bufferSize != 0) {
            BufferDesc bufferDesc;
            bufferDesc.size = materialDesc.bufferSize;
            bufferDesc.usage = BufferUsage::Uniform;
            bufferDesc.isDynamic = true;

            data.ubo = renderDevice->CreateBuffer(bufferDesc);
        }

        data.resourceSet = renderDevice->CreateResourceSet(shader.resourceSetLayout);

        const MaterialHandle handle{.Id = static_cast<uint32_t>(materials.size())};

        materials.push_back(data);

        if (materialDesc.bufferSize != 0) {
            BindBufferToMaterial(handle, 3, data.ubo);
            BindTextureToMaterial(handle, 6, shadowTexture);
        }

        return handle;
    }

    ShaderHandle Renderer::CreateShader(const ShaderDesc &shaderDesc) {
        GraphicsShader graphicsShader;

        const RenderFace renderFace = shaderDesc.RenderFace;
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
        pipelineStateDesc.RasterizerState.renderFace = renderFace;

        const auto it = std::ranges::find_if(shaderDesc.resourceSetLayouts, [](const ResourceSetLayoutDesc &u) {
            return u.set_index == 2;
        });

        if (it != shaderDesc.resourceSetLayouts.end()) {
            graphicsShader.resourceSetLayout = renderDevice->CreateResourceSetLayout(*it);
        }

        graphicsShader.pipeline = renderDevice->CreatePipeline(shaderDesc, pipelineStateDesc);

        shaders.push_back(graphicsShader);

        return ShaderHandle{.Id = shaders.size() - 1};
    }

    Mesh *Renderer::CreateMesh(const std::vector<Vertex> &vertices, const std::vector<uint16_t> &indices) {
        MeshBuffer meshBuffer;

        meshBuffer.vertexData = malloc(vertices.size() * sizeof(Vertex));
        meshBuffer.indexData = malloc(indices.size() * sizeof(uint16_t));

        memcpy(meshBuffer.vertexData, vertices.data(), vertices.size() * sizeof(Vertex));

        uploadCommandBuffer->UploadBufferData(staticMeshes.vertexBuffer, meshBuffer.vertexData,
                                              staticMeshes.currentVertexOffset, vertices.size() * sizeof(Vertex));

        meshBuffer.vertexBuffer.offset = staticMeshes.currentVertexOffset;
        meshBuffer.vertexBuffer.size = vertices.size() * sizeof(Vertex);

        staticMeshes.currentVertexOffset += vertices.size() * sizeof(Vertex);

        memcpy(meshBuffer.indexData, indices.data(), indices.size() * sizeof(uint16_t));

        uploadCommandBuffer->UploadBufferData(staticMeshes.indexBuffer, meshBuffer.indexData,
                                              staticMeshes.currentIndexOffset, indices.size() * sizeof(uint16_t));

        meshBuffer.indexBuffer.offset = staticMeshes.currentIndexOffset;
        meshBuffer.indexBuffer.size = indices.size() * sizeof(uint16_t);

        staticMeshes.currentIndexOffset += indices.size() * sizeof(uint16_t);

        const auto mesh = new Mesh(vertices, indices);
        mesh->vertexBuffer = staticMeshes.vertexBuffer;
        mesh->indexBuffer = staticMeshes.indexBuffer;
        mesh->indexMesh = indexMesh++;
        meshes.push_back(meshBuffer);
        mesh->SetId(MeshId{.Id = meshes.size() - 1});
        return mesh;
    }

    LightHandle Renderer::CreateLight(const LightType type) {
        LightHandle handle{};

        switch (type) {
            case LightType::Ambient: {
                LightItemData itemData;
                itemData.index = 0;
                itemData.type = type;
                lights.push_back(itemData);
                handle = LightHandle{.id = lights.size() - 1};
                break;
            }
            case LightType::Directional: {
                LightItemData itemData;
                itemData.index = 0;
                itemData.type = type;
                lights.push_back(itemData);
                handle = LightHandle{.id = lights.size() - 1};
                break;
            }
            case LightType::Point: {
                LightItemData itemData;
                itemData.index = ++lastPointLightIndex;
                itemData.type = type;
                lights.push_back(itemData);
                handle = LightHandle{.id = lights.size() - 1};
                break;
            }
            case LightType::Spot: {
                LightItemData itemData;
                itemData.index = ++lastSpotLightIndex;
                itemData.type = type;
                lights.push_back(itemData);
                handle = LightHandle{.id = lights.size() - 1};
                break;
            }
        }

        return handle;
    }

    void Renderer::BindTextureToMaterial(const MaterialHandle material, const uint32_t slot,
                                         const TextureHandle texture) const {
        const MaterialData materialData = materials[material.Id];
        renderDevice->UpdateResourceSet(materialData.resourceSet, slot, texture);
    }

    void Renderer::BindTextureToMaterial(const Material &material, const uint32_t slot, const Texture &texture) {
        const MaterialData materialData = materials[material.GetId().Id];
        if (slot == 15) {
            renderDevice->UpdateResourceSet(materialData.resourceSet, slot, brdfTexture);
            return;
        }
        renderDevice->UpdateResourceSet(materialData.resourceSet, slot, texture.GetId());
    }

    void Renderer::BindCubemapToMaterial(const Material &material, const uint32_t slot,
                                         const Cubemap &cubemap) {
        const MaterialData m = materials[material.GetId().Id];
        if (slot == 13) {
            renderDevice->UpdateResourceSet(m.resourceSet, slot, irradianceCubeMap);
            return;
        }
        if (slot == 14) {
            renderDevice->UpdateResourceSet(m.resourceSet, slot, prefilterCubeMap);
            return;
        }
        if (slot == 3) {
            renderDevice->UpdateResourceSet(m.resourceSet, slot, irradianceCubeMap);
            return;
        }
        renderDevice->UpdateResourceSet(m.resourceSet, slot, cubemap.GetHandle());
    }

    void Renderer::BindBufferToMaterial(const MaterialHandle materialHandle, const uint32_t slot,
                                        const BufferHandle bufferId) {
        const MaterialData material = materials[materialHandle.Id];
        renderDevice->UpdateResourceSet(material.resourceSet, slot, bufferId);
    }

    void Renderer::BeginFrame(Camera &camera) {
        activeCamera = &camera;
    }

    void Renderer::Render(Scene &scene) {
        const auto cmd = renderDevice->AllocateCommandBuffer(CommandBufferType::Graphics);

        cmd->Begin();

        std::vector<DrawCall> drawCalls = scene.GetDrawCalls();
        const std::vector<Light *> lights = scene.GetLights();
        std::unordered_map<Material *, DrawCall> _materialsGrouped;

        for (const auto drawCall: drawCalls) {
            ModelBuffer modelBuffer;
            modelBuffer.model = drawCall.transform->GetWorldMatrix();
            renderDevice->UpdateBuffer(this->modelBuffer, &modelBuffer, drawCall._mesh->indexMesh * sizeof(ModelBuffer),
                                       sizeof(ModelBuffer));
            _materialsGrouped.emplace(drawCall.material, drawCall);
        }

        auto _materials = std::views::keys(_materialsGrouped);
        std::vector keys(_materials.begin(), _materials.end());

        for (const auto material: keys) {
            if (material->IsDirty()) {
                cmd->UpdateBuffer(materials[material->GetId().Id].ubo, material->GetBufferData().data(), 0,
                                  material->GetBufferData().size());
                material->ClearDirty();
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

        // Shadow pass

        CameraBuffer cameraBuffer;
        cameraBuffer.view = lights[1]->GetGameObject()->GetTransform()->LookAt();
        cameraBuffer.proj = lights[1]->GetGameObject()->GetTransform()->Othographic(
            -50.0f, 50.0f, -50.0f, 50.0f, 0.1f, 100.0f);
        cameraBuffer.position = Vector4(lights[1]->GetGameObject()->GetTransform()->position, 1.0f);

        lightRenderData.data.directionalLight.space = cameraBuffer.proj * cameraBuffer.view;

        renderDevice->UpdateBuffer(lightRenderData.buffer, &lightRenderData.data, 0, sizeof(LightData));
        renderDevice->UpdateBuffer(lightSpace.ViewProjectionBuffer, &cameraBuffer, 0, sizeof(CameraBuffer));

        cmd->BeginRenderPass(renderPassesDesc[1]);
        cmd->BindPipeline(shaders[depthShader->GetHandle().Id].pipeline);
        cmd->BindResourceSet(lightSpace.ResourceSet, mainPipelineLayout, 0);
        cmd->BindResourceSet(modelResourceSet, mainPipelineLayout, 1, 0);
        cmd->BindVertexBuffer(staticMeshes.vertexBuffer, 0, sizeof(Vertex));
        cmd->BindIndexBuffer(staticMeshes.indexBuffer);

        for (const auto drawCall: drawCalls) {
            MeshBuffer meshBuffer = meshes[drawCall._mesh->GetId().Id];
            cmd->DrawIndexed(drawCall._mesh->GetIndexCount(), meshBuffer.indexBuffer.offset / sizeof(uint16_t),
                             meshBuffer.vertexBuffer.offset / sizeof(Vertex), drawCall._mesh->indexMesh);
        }

        cmd->EndRenderPass();

        // raster pass

        cameraBuffer.view = activeCamera->GetTransform()->LookAt();
        cameraBuffer.proj = activeCamera->GetProjection();
        cameraBuffer.position = Vector4(activeCamera->GetTransform()->position, 1.0f);

        renderDevice->UpdateBuffer(cameras[activeCamera->GetHandle().Id].ViewProjectionBuffer, &cameraBuffer, 0,
                                   sizeof(CameraBuffer));

        cmd->BeginRenderPass(renderPassesDesc[0]);
        cmd->BindResourceSet(cameras[activeCamera->GetHandle().Id].ResourceSet, mainPipelineLayout, 0);

        std::ranges::sort(drawCalls, [](const DrawCall &a, const DrawCall &b) {
            return a.material->GetMaterialHandle().Id < b.material->GetMaterialHandle().Id;
        });

        const Material *lastMaterial = nullptr;
        for (const auto drawCall: drawCalls) {
            // material properties
            if (lastMaterial != drawCall.material) {
                lastMaterial = drawCall.material;
                cmd->BindPipeline(shaders[materials[lastMaterial->GetMaterialHandle().Id].shader.Id].pipeline);
                cmd->BindResourceSet(materials[lastMaterial->GetMaterialHandle().Id].resourceSet,
                                     materials[lastMaterial->GetMaterialHandle().Id].pipeline, 2);
            }

            MeshBuffer meshBuffer = meshes[drawCall._mesh->GetId().Id];
            cmd->DrawIndexed(drawCall._mesh->GetIndexCount(), meshBuffer.indexBuffer.offset / sizeof(uint16_t),
                             meshBuffer.vertexBuffer.offset / sizeof(Vertex), drawCall._mesh->indexMesh);
        }

        cmd->EndRenderPass();

        //skybox

        cameraBuffer.view = activeCamera->GetTransform()->LookAt();
        cameraBuffer.proj = activeCamera->GetProjection();
        cameraBuffer.position = Vector4(activeCamera->GetTransform()->position, 1.0f);

        renderDevice->UpdateBuffer(cameras[activeCamera->GetHandle().Id].ViewProjectionBuffer, &cameraBuffer, 0,
                                   sizeof(CameraBuffer));

        cmd->BeginRenderPass(renderPassesDesc[2]);
        cmd->BindPipeline(shaders[materials[skyboxMaterial->GetMaterialHandle().Id].shader.Id].pipeline);
        cmd->BindResourceSet(cameras[activeCamera->GetHandle().Id].ResourceSet, mainPipelineLayout, 0);
        cmd->BindResourceSet(materials[skyboxMaterial->GetMaterialHandle().Id].resourceSet,
                             shaders[materials[skyboxMaterial->GetMaterialHandle().Id].shader.Id].pipeline, 2);
        cmd->DrawIndexed(36, 0, 0, 0);
        cmd->EndRenderPass();

        // end skybox

        cmd->End();

        if (uploadCommandBuffer != nullptr) {
            uploadCommandBuffer->End();
            renderDevice->SubmitCommandBuffer(uploadCommandBuffer);
            uploadCommandBuffer = nullptr;
        }

        renderDevice->SubmitCommandBuffer(cmd);
        delete cmd;
    }

    void Renderer::Bake(DrawCall &irredianceDrawCall, DrawCall &prefilterDrawCall, DrawCall &brdfDrawCall) {
        const auto cmd = renderDevice->AllocateCommandBuffer(CommandBufferType::Graphics);

        cmd->Begin();

        Matrix4x4 captureProjection = Matrix4x4::Perspective(90.0f, 1.0f, 0.1f, 100.0f);
        const Matrix4x4 captureViews[] =
        {
            Matrix4x4::LookAt(Vector3::Zero(), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f)),
            Matrix4x4::LookAt(Vector3::Zero(), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f)),
            Matrix4x4::LookAt(Vector3::Zero(), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f)),
            Matrix4x4::LookAt(Vector3::Zero(), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f)),
            Matrix4x4::LookAt(Vector3::Zero(), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, -1.0f, 0.0f)),
            Matrix4x4::LookAt(Vector3::Zero(), Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, -1.0f, 0.0f)),
        };

        CameraBoxBuffer cameraBoxBufferData;
        cameraBoxBufferData.proj = Matrix4x4(captureProjection);
        cameraBoxBufferData.view[0] = Matrix4x4(captureViews[0]);
        cameraBoxBufferData.view[1] = Matrix4x4(captureViews[1]);
        cameraBoxBufferData.view[2] = Matrix4x4(captureViews[2]);
        cameraBoxBufferData.view[3] = Matrix4x4(captureViews[3]);
        cameraBoxBufferData.view[4] = Matrix4x4(captureViews[4]);
        cameraBoxBufferData.view[5] = Matrix4x4(captureViews[5]);

        renderDevice->UpdateBuffer(cameraBoxBuffer, &cameraBoxBufferData, 0,
                                   sizeof(CameraBoxBuffer));

        // irradiance
        cmd->BeginRenderPass(renderPassesDesc[3]);
        cmd->BindPipeline(shaders[materials[irredianceDrawCall.material->GetId().Id].shader.Id].pipeline);
        cmd->BindResourceSet(cameraBoxResourceSet,
                            shaders[materials[irredianceDrawCall.material->GetId().Id].shader.Id].pipeline, 0);
        cmd->BindResourceSet(materials[irredianceDrawCall.material->GetId().Id].resourceSet,
                             shaders[materials[irredianceDrawCall.material->GetId().Id].shader.Id].pipeline, 2);
        cmd->BindVertexBuffer(staticMeshes.vertexBuffer, 0, sizeof(Vertex));
        cmd->BindVertexBuffer(instanceBuffer, 1, sizeof(int));
        cmd->BindIndexBuffer(staticMeshes.indexBuffer);
        cmd->DrawIndexed(36, 0, 0, 0, 6);
        cmd->EndRenderPass();

        //prefilter
        for (int mip = 0; mip < 4; mip++) {
            const float roughness = static_cast<float>(mip) / static_cast<float>(4);
            prefilterDrawCall.material->SetProperty("roughness", roughness);
            renderDevice->UpdateBuffer(materials[prefilterDrawCall.material->GetId().Id].ubo, prefilterDrawCall.material->GetBufferData().data(), 0,
                                   prefilterDrawCall.material->GetBufferData().size());
            cmd->BeginRenderPass(renderPassesDesc[4 + mip]);
            cmd->BindPipeline(shaders[materials[prefilterDrawCall.material->GetId().Id].shader.Id].pipeline);
            cmd->BindResourceSet(materials[prefilterDrawCall.material->GetId().Id].resourceSet,
                                 shaders[materials[prefilterDrawCall.material->GetId().Id].shader.Id].pipeline, 2);
            cmd->DrawIndexed(36, 0, 0, 0, 6);
            cmd->EndRenderPass();
        }

        //brdf
        cmd->BeginRenderPass(renderPassesDesc[8]);
        cmd->BindPipeline(shaders[materials[brdfDrawCall.material->GetId().Id].shader.Id].pipeline);
        cmd->DrawIndexed(6, 36, 24, 0);
        cmd->EndRenderPass();

        cmd->End();

        if (uploadCommandBuffer != nullptr) {
            std::vector<int> customInstanceIDs(10000);
            for(int i = 0; i < 10000; i++) {
                customInstanceIDs[i] = i;
            }
            uploadCommandBuffer->UploadBufferData(instanceBuffer, customInstanceIDs.data(), 0, customInstanceIDs.size() * sizeof(int));
            uploadCommandBuffer->End();
            renderDevice->SubmitCommandBuffer(uploadCommandBuffer);
            uploadCommandBuffer = nullptr;
        }

        renderDevice->SubmitCommandBuffer(cmd);
        delete cmd;
    }

    void Renderer::EndFrame() {
        renderDevice->Present();
    }

    void Renderer::SetDepthShader(Shader &shader) {
        depthShader = &shader;
    }

    void Renderer::SetSkyboxMaterial(Material &material) {
        skyboxMaterial = &material;
    }
} // namespace Rendering
