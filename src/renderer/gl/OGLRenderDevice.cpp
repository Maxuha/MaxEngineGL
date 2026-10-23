//
// Created by zykov on 5/1/2026.
//

#include "OGLRenderDevice.h"
#include "CommandBuffer.h"
#include "OGLInstance.h"
#include "OGLRenderPassManager.h"
#include "OGLShaderManager.h"
#include "OGLTextureManager.h"
#include "OGLCommandPoolManager.h"
#include "OGLDescriptorSetManager.h"
#include "OGLPipelineManager.h"
#include "OGLVertexBufferManager.h"
#include "stb_image_write.h"
#include "../../math/Color.h"

#include <algorithm>
#include <cmath>
#include <limits>


namespace Rendering {
    bool SaveGLTextureToDisk(GLuint textureID, const char *filename) {
        if (textureID == 0 || !glIsTexture(textureID)) {
            std::cerr << "Error: Invalid texture id " << textureID << "\n";
            return false;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);

        GLint width = 0;
        GLint height = 0;
        GLint internalFormat = 0;

        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);

        if (width <= 0 || height <= 0) {
            std::cerr << "Error: Invalid texture dimensions (" << width << "x" << height << ")\n";
            glBindTexture(GL_TEXTURE_2D, 0);
            return false;
        }

        const bool isDepth =
                internalFormat == GL_DEPTH_COMPONENT ||
                internalFormat == GL_DEPTH_COMPONENT16 ||
                internalFormat == GL_DEPTH_COMPONENT24 ||
                internalFormat == GL_DEPTH_COMPONENT32 ||
                internalFormat == GL_DEPTH_COMPONENT32F ||
                internalFormat == GL_DEPTH24_STENCIL8 ||
                internalFormat == GL_DEPTH32F_STENCIL8;

        std::vector<unsigned char> pixels;
        int channels = 4;

        GLint previousPackAlignment = 0;
        glGetIntegerv(GL_PACK_ALIGNMENT, &previousPackAlignment);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);

        if (isDepth) {
            std::vector<float> depth(static_cast<size_t>(width) * height);
            glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, depth.data());

            float minDepth = std::numeric_limits<float>::max();
            float maxDepth = std::numeric_limits<float>::lowest();

            for (const float value: depth) {
                if (!std::isfinite(value)) {
                    continue;
                }

                minDepth = std::min(minDepth, value);
                maxDepth = std::max(maxDepth, value);
            }

            pixels.resize(depth.size());
            channels = 1;

            const bool hasRange = minDepth < maxDepth;
            const float range = hasRange ? (maxDepth - minDepth) : 1.0f;

            for (size_t i = 0; i < depth.size(); ++i) {
                const float rawValue = std::clamp(depth[i], 0.0f, 1.0f);
                const float normalized = hasRange ? ((rawValue - minDepth) / range) : rawValue;
                pixels[i] = static_cast<unsigned char>(std::clamp(normalized, 0.0f, 1.0f) * 255.0f);
            }

            std::cout << "Depth texture stats for " << filename
                    << ": min=" << minDepth
                    << ", max=" << maxDepth
                    << ", normalized=" << (hasRange ? "yes" : "no")
                    << "\n";
        } else {
            GLenum format = GL_RGBA;

            if (internalFormat == GL_RGB || internalFormat == GL_RGB8 || internalFormat == GL_SRGB8) {
                format = GL_RGB;
                channels = 3;
            } else if (
                internalFormat == GL_RGBA ||
                internalFormat == GL_RGBA8 ||
                internalFormat == GL_SRGB8_ALPHA8
            ) {
                format = GL_RGBA;
                channels = 4;
            }

            pixels.resize(static_cast<size_t>(width) * height * channels);
            glGetTexImage(GL_TEXTURE_2D, 0, format, GL_UNSIGNED_BYTE, pixels.data());
        }

        glPixelStorei(GL_PACK_ALIGNMENT, previousPackAlignment);
        glBindTexture(GL_TEXTURE_2D, 0);

        // stbi_flip_vertically_on_write(true);

        const int success = stbi_write_png(filename, width, height, channels, pixels.data(), width * channels);

        if (!success) {
            std::cerr << "Error: Failed to write image file: " << filename << "\n";
            return false;
        }

        std::cout << "Successfully saved texture (" << width << "x" << height << ") to " << filename << "\n";
        return true;
    }

    OGLRenderDevice::OGLRenderDevice(IWindow &window) {
        this->window = static_cast<GLFWwindow *>(window.GetHandle());

        instance = new OGLInstance(window);
        textureManager = new OGLTextureManager();
        cubeMapManager = new OGlCubeMapManager();
        pipelineManager = new OGLPipelineManager();
        shaderManager = new OGLShaderManager();
        bufferManager = new OGLVertexBufferManager();
        commandPoolManager = new OGLCommandPoolManager();
        descriptorSetManager = new OGLDescriptorSetManager();
    }

    OGLRenderDevice::~OGLRenderDevice() = default;

    CommandBuffer *OGLRenderDevice::AllocateCommandBuffer(const CommandBufferType type) {
        return new CommandBuffer(9048 * 1024, type);
    }

    void OGLRenderDevice::CreateVao(const VertexLayout layout) {
        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        for (const auto &binding : layout.bindings) {
            const GLuint divisor = binding.stepRate == VertexStepRate::PerInstance ? 1 : 0;
            glVertexArrayBindingDivisor(vao, binding.bufferIndex, divisor);
        }

        for (const auto &attribute : layout.attributes) {
            glEnableVertexArrayAttrib(vao, attribute.location);
            glVertexArrayAttribFormat(vao, attribute.location, MapVertexFormat(attribute.format), GL_FLOAT, GL_FALSE, attribute.offset);
            glVertexArrayAttribBinding(vao, attribute.location, attribute.bufferIndex);
        }

        glBindVertexArray(0);
    }

    TextureHandle OGLRenderDevice::CreateTexture(const TextureCreateRequest &desc, const void *initialData) {
        const auto _data = const_cast<void *>(initialData);

        const TextureHandle textureHandle = textureManager->CreateTexture(desc.Width, desc.Height, desc.Format, _data);
        OGLTexture &texture = textureManager->GetTexture(textureHandle);

        if (desc.Format == TextureFormat::DEPTH) {
            GLuint fbo;
            glCreateFramebuffers(1, &fbo);
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            // glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, texture.Id, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            texture.fbo = fbo;
        }

        return textureHandle;

        GLuint glId;

        glCreateTextures(GL_TEXTURE_2D, 1, &glId);

        const GLenum internalFormat = MapTextureInternalFormat(desc.Format);

        auto mips = static_cast<GLsizei>(desc.MipLevels);

        if (mips == 0) {
            if (initialData) {
                mips = static_cast<GLsizei>(std::log2(std::max(desc.Width, desc.Height))) + 1;
            } else {
                mips = 1;
            }
        }

        glTextureStorage2D(glId, mips, internalFormat, static_cast<GLsizei>(desc.Width),
                           static_cast<GLsizei>(desc.Height));

        if (initialData) {
            const GLenum externalFormat = MapTextureExternalFormat(desc.Format);

            glTextureSubImage2D(
                glId,
                0,
                0, 0,
                static_cast<GLsizei>(desc.Width),
                static_cast<GLsizei>(desc.Height),
                externalFormat,
                GL_UNSIGNED_BYTE,
                initialData
            );
        }

        if (mips > 1) {
            glGenerateTextureMipmap(glId);
        }

        if (mips > 1) {
            glTextureParameteri(glId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTextureParameteri(glId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        } else {
            glTextureParameteri(glId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(glId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(glId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTextureParameteri(glId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

            constexpr float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

            glTextureParameteri(glId, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
            glTextureParameteri(glId, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

            // glTextureParameteri(glId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }

        const auto id = TextureHandle{.Id = static_cast<uint32_t>(textures.size())};

        TextureData data;
        data.id = glId;
        data.width = desc.Width;
        data.height = desc.Height;
        data.format = desc.Format;
        data.mipLevels = mips;
        textures.push_back(data);

        return id;
    }

    CubeMapHandle OGLRenderDevice::CreateCubeMap(const TextureCreateRequest &desc, const std::vector<void *> &datas) {
        return cubeMapManager->CreateCubeMap(desc, datas);
    }

    PipelineLayoutHandle OGLRenderDevice::CreatePipelineLayout(
        const std::vector<ResourceSetLayoutHandle> &resourceSetLayouts) {
        OGLPipelineLayout layout;
        return {};
    }

    ResourceSetLayoutHandle
    OGLRenderDevice::CreateResourceSetLayout(const ResourceSetLayoutDesc &resourceSetLayoutDesc) {
        OGLDescriptorSetLayout layout;
        layout.set_index = resourceSetLayoutDesc.set_index;
        for (const auto binding: resourceSetLayoutDesc.bindings) {
            OGLDescriptorSetLayoutItem item;
            item.binding_slot = binding.binding_slot;
            item.stage_flags = binding.stage_flags;
            item.type = binding.type;
            layout.bindings.push_back(item);
        }
        return descriptorSetManager->CreateDescriptorSetLayout(layout);
    }

    ResourceSetHandle OGLRenderDevice::CreateResourceSet(const ResourceSetLayoutHandle resourceSetLayoutHandle) {
        const OGLDescriptorSetLayout layout = descriptorSetManager->GetDescriptorSetLayout(resourceSetLayoutHandle);
        return descriptorSetManager->CreateDescriptorSet(layout);
    }

    void OGLRenderDevice::UpdateResourceSet(const ResourceSetHandle setHandle, const uint32_t binding,
                                            const TextureHandle textureHandle) {
        descriptorSetManager->BindTexture(setHandle, binding, textureHandle);
    }

    void OGLRenderDevice::UpdateResourceSet(const ResourceSetHandle setHandle, const uint32_t binding,
        const CubeMapHandle cubemapHandle) {
        descriptorSetManager->BindCubemap(setHandle, binding, cubemapHandle);
    }


    void OGLRenderDevice::UpdateResourceSet(const ResourceSetHandle setHandle, const uint32_t binding,
                                            const BufferHandle bufferHandle) {
        descriptorSetManager->BindBuffer(setHandle, binding, bufferHandle);
    }

    BufferHandle OGLRenderDevice::CreateBuffer(const BufferDesc &desc) {
        return bufferManager->CreateBuffer(desc);
    }

    void OGLRenderDevice::UpdateBuffer(const BufferHandle id, const void *data, const int offset, const int size) {
        // TODO Check if buffer is available for update
        // glNamedBufferSubData(buffers[id.Id], static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size), data);

        bufferManager->UpdateUniformBuffer(id, data, offset, size);
    }

    void OGLRenderDevice::DestroyBuffer(const BufferHandle id) {
        glDeleteBuffers(1, &buffers[id.Id]);
    }

    PipelineHandle OGLRenderDevice::CreatePipeline(const ShaderDesc &shaderDesc,
                                                   const PipelineStateDesc &pipelineStateDesc) {
        const ShaderHandle vShaderHandle = shaderManager->CreateShader(ShaderType::Vertex, shaderDesc.vertexCode);
        const ShaderHandle fShaderHandle = shaderManager->CreateShader(ShaderType::Fragment, shaderDesc.fragmentCode);

        OGLGraphicsShader shader;
        shader.vShader = shaderManager->GetShader(vShaderHandle).handle;
        shader.fShader = shaderManager->GetShader(fShaderHandle).handle;

        for (auto resource_set_layout: shaderDesc.resourceSetLayouts) {
            OGLDescriptorSetLayout layout;
            layout.set_index = resource_set_layout.set_index;
            for (const auto binding: resource_set_layout.bindings) {
                OGLDescriptorSetLayoutItem item;
                item.binding_slot = binding.binding_slot;
                item.stage_flags = binding.stage_flags;
                item.type = binding.type;
                layout.bindings.push_back(item);
            }
            shader.layouts.push_back(layout);
        }

        _graphicsShaders.push_back(shader);

        const auto shaderHandle = ShaderHandle{.Id = static_cast<uint32_t>(_graphicsShaders.size() - 1)};

        return pipelineManager->CreatePipeline(shader, pipelineStateDesc);
    }

    void OGLRenderDevice::SubmitCommandBuffer(CommandBuffer *commandBuffer) {
        this->commandBuffer = commandBuffer;
        std::vector<uint8_t> commands = commandBuffer->GetCommands();
        size_t readOffset = 0;
        size_t head = commandBuffer->GetHead();

        while (readOffset < head) {
            const GLCommandType type = *reinterpret_cast<GLCommandType *>(&commands[readOffset]);
            readOffset += sizeof(GLCommandType);

            switch (type) {
                case GLCommandType::BeginRenderPass: {
                    const auto *cmd = reinterpret_cast<GLCommand_BeginRenderPass *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_BeginRenderPass);

                    GLuint activeFBO = 0;

                    std::optional<AttachmentDescription> colorAttachment = cmd->colorAttachment;
                    std::optional<AttachmentDescription> depthAttachment = cmd->depthAttachment;

                    if (depthAttachment.has_value() && depthAttachment.value().renderTarget.has_value() && !
                        colorAttachment.has_value()) {
                        OGLTexture texture = textureManager->GetTexture(depthAttachment->renderTarget.value());
                        activeFBO = texture.fbo;
                    }

                    glBindFramebuffer(GL_FRAMEBUFFER, activeFBO);

                    Rect viewport = cmd->viewport;

                    glViewport(viewport.x, viewport.y, viewport.width, viewport.height);

                    if (activeFBO != 0) {
                        glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);

                        GLenum attachmentType = MapAttachment(depthAttachment.value().type);
                        OGLTexture texture = textureManager->GetTexture(depthAttachment.value().renderTarget.value());

                        glNamedFramebufferTexture(activeFBO, attachmentType, texture.Id, 0);

                        if (colorAttachment.has_value()) {
                            glNamedFramebufferDrawBuffer(activeFBO, GL_COLOR_ATTACHMENT0);
                            glNamedFramebufferReadBuffer(activeFBO, GL_COLOR_ATTACHMENT0);
                        } else {
                            glNamedFramebufferDrawBuffer(activeFBO, GL_NONE);
                            glNamedFramebufferReadBuffer(activeFBO, GL_NONE);
                        }
                    } else {
                        glClipControl(GL_UPPER_LEFT, GL_ZERO_TO_ONE);
                    }

                    GLbitfield clearMask = 0;
                    if (colorAttachment.has_value() && colorAttachment.value().loadOp == AttachmentLoadOp::CLEAR) {
                        if (colorAttachment.value().type == AttachmentType::COLOR) clearMask |= GL_COLOR_BUFFER_BIT;

                        Color clearColor = colorAttachment.value().clearColor;

                        glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
                    }

                    if (depthAttachment.has_value()) {
                        if (depthAttachment.value().loadOp == AttachmentLoadOp::CLEAR) {
                            if (depthAttachment.value().type == AttachmentType::DEPTH) clearMask |= GL_DEPTH_BUFFER_BIT;

                            glClearDepth(depthAttachment.value().depthClearValue);
                        }
                    }


                    if (clearMask != 0) {
                        //glClearStencil(0);
                        glClear(clearMask);
                    }

                    break;
                }

                case GLCommandType::EndRenderPass: {
                    const auto *cmd = reinterpret_cast<GLCommand_EndRenderPass *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_EndRenderPass);

                    break;
                }

                case GLCommandType::BindPipeline: {
                    const auto *cmd = reinterpret_cast<GLCommand_BindPipeline *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_BindPipeline);

                    currentPipeline = cmd->pipeline;

                    const auto pipeline = pipelineManager->GetPipeline(currentPipeline);

                    glUseProgram(pipeline.program);
                    pipeline.cullEnable ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
                    pipeline.depthTestEnable ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
                    glDepthMask(pipeline.depthWriteEnable);
                    glDepthFunc(pipeline.depthCompareFunc);
                    glCullFace(pipeline.cullMode);
                    break;
                }

                case GLCommandType::BindVertexBuffer: {
                    const auto *cmd = reinterpret_cast<GLCommand_BindBuffer *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_BindBuffer);

                    const auto pipeline = pipelineManager->GetPipeline(currentPipeline);
                    const auto &buffer = bufferManager->GetBuffer(cmd->buffer);

                    glVertexArrayVertexBuffer(1, 0,
                                              buffer.glId, 0, sizeof(Vertex));
                    break;
                }

                case GLCommandType::BindIndexBuffer: {
                    const auto *cmd = reinterpret_cast<GLCommand_BindBuffer *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_BindBuffer);

                    const auto pipeline = pipelineManager->GetPipeline(currentPipeline);
                    const auto &buffer = bufferManager->GetBuffer(cmd->buffer);

                    glVertexArrayElementBuffer(1, buffer.glId);
                    break;
                }

                case GLCommandType::UpdateBuffer: {
                    const auto *cmd = reinterpret_cast<GLCommand_UpdateBuffer *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_UpdateBuffer);

                    const auto bufferHandle = cmd->buffer;
                    const auto data = cmd->data;
                    const auto offset = cmd->offset;
                    const auto size = cmd->size;

                    OGLBuffer &buffer = bufferManager->GetBuffer(bufferHandle);

                    glNamedBufferSubData(buffer.glId, static_cast<GLintptr>(offset),
                                         static_cast<GLsizeiptr>(size), data);
                    break;
                }

                case GLCommandType::BindResourceSet: {
                    const auto *cmd = reinterpret_cast<GLCommand_BindResourceSet *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_BindResourceSet);

                    OGLResourceSet resourceSet = descriptorSetManager->GetResourceSet(cmd->resourceId);

                    for (auto item: resourceSet.items) {
                        glBindTextureUnit(item.binding, 0);

                        //TODO Remove it
                        if (item.bufferHandle.Id == 9999999 && item.textureHandle.Id == 9999999 && item.cubemapHandle.Id == 9999999) continue;

                        if (item.cubemapHandle.Id != 9999999) {
                            OGLCubeMapData cubemap = cubeMapManager->GetCubeMap(item.cubemapHandle);
                            glBindTextureUnit(item.binding, cubemap.glId);
                            continue;
                        }

                        if (item.IsBuffer()) {
                            OGLBuffer &buffer = bufferManager->GetBuffer(item.bufferHandle);
                            if (item.binding == 2) {
                                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, item.binding, buffer.glId);
                            } else {
                                glBindBufferBase(GL_UNIFORM_BUFFER, item.binding, buffer.glId);
                            }
                        } else {
                            OGLTexture texture = textureManager->GetTexture(item.textureHandle);
                            glBindTextureUnit(item.binding, texture.Id);
                        }
                    }
                    break;
                }

                case GLCommandType::PushConstants: {
                    const auto *cmd = reinterpret_cast<GLCommand_PushConstants *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_PushConstants);
                    break;
                }

                case GLCommandType::Draw: {
                    const auto *cmd = reinterpret_cast<GLCommand_Draw *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_Draw);

                    glBindVertexArray(1);
                    glDrawArrays(GL_TRIANGLES, 0, cmd->vertexCount);
                    glBindVertexArray(0);
                    break;
                }

                case GLCommandType::DrawIndexed: {
                    const auto *cmd = reinterpret_cast<GLCommand_DrawIndexed *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_DrawIndexed);

                    uint32_t indexCount = cmd->indexCount;
                    uint32_t firstIndex = cmd->firstIndex;
                    uint32_t vertexOffset = cmd->vertexOffset;
                    uint32_t indexMesh = cmd->indexMesh;

                    const auto pipeline = pipelineManager->GetPipeline(currentPipeline);

                    glBindVertexArray(1);

                    // glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(cmd->indexCount), GL_UNSIGNED_SHORT, nullptr);

                    // glDrawElementsInstancedBaseInstance(
                    //     GL_TRIANGLES,
                    //     cmd->indexCount,
                    //     GL_UNSIGNED_SHORT,
                    //     nullptr,
                    //     1,
                    //     cmd->indexMesh
                    // );

                    //should be system uint16_t
                    uintptr_t byteOffset = static_cast<uintptr_t>(firstIndex) * sizeof(uint16_t);
                    auto glIndicesPointer = reinterpret_cast<void*>(byteOffset);

                    glDrawElementsInstancedBaseVertexBaseInstance(
                        GL_TRIANGLES,
                        indexCount,
                        GL_UNSIGNED_SHORT,
                        glIndicesPointer,
                        1,
                        vertexOffset,
                        indexMesh
                    );

                    glBindVertexArray(0);
                    break;
                }

                case GLCommandType::UploadBufferData: {
                    const auto *cmd = reinterpret_cast<GLCommand_UploadBufferData *>(&commands[readOffset]);
                    // readOffset += sizeof(GLCommand_UploadBufferData) + cmd->size;
                    readOffset += sizeof(GLCommand_UploadBufferData);

                    auto bufferHandle = cmd->buffer;
                    // auto data = reinterpret_cast<const uint8_t *>(cmd) + sizeof(GLCommand_UploadBufferData);
                    auto data = cmd->data;
                    auto size = cmd->size;
                    auto offset = cmd->offset;

                    OGLBuffer &buffer = bufferManager->GetBuffer(bufferHandle);

                    glNamedBufferSubData(buffer.glId, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size),
                                         data);
                }

                default:
                    break;
            }
        }

        head = 0;
    }

    void OGLRenderDevice::Present() {
        glfwSwapBuffers(window);
    }

    TextureFormat OGLRenderDevice::GetDisplayFormat() {
        return TextureFormat::SRGBA;
    }

    void OGLRenderDevice::SetupVertexLayoutForPSO(const GLuint vao, const VertexLayout &layout) {
        for (const auto &b: layout.bindings) {
            const GLuint divisor = (b.stepRate == VertexStepRate::PerInstance) ? 1 : 0;
            glVertexArrayBindingDivisor(vao, b.bufferIndex, divisor);
        }

        for (const auto &attr: layout.attributes) {
            glEnableVertexArrayAttrib(vao, attr.location);

            constexpr GLenum glType = GL_FLOAT;
            const GLint components = MapGLComponents(attr.format);
            constexpr GLboolean normalized = GL_FALSE;

            glVertexArrayAttribFormat(vao, attr.location, components, glType, normalized, attr.offset);

            glVertexArrayAttribBinding(vao, attr.location, attr.bufferIndex);
        }
    }

    GLint OGLRenderDevice::MapGLComponents(const VertexFormat format) {
        switch (format) {
            case VertexFormat::Float3: return 3;
            case VertexFormat::Float4: return 4;
            case VertexFormat::Float2: return 2;
            case VertexFormat::Int4: return 4;
            case VertexFormat::UByte4N: return 4;
            default: return 0;
        }
    }

    GLenum OGLRenderDevice::MapTextureInternalFormat(const TextureFormat format) {
        switch (format) {
            case TextureFormat::RGBA: return GL_RGBA8;
            case TextureFormat::RGB: return GL_RGB8;
            case TextureFormat::DEPTH: return GL_DEPTH_COMPONENT24;
            case TextureFormat::SRGBA: return GL_SRGB8_ALPHA8;
                break;
        }
        return GL_RGBA8;
    }

    GLenum OGLRenderDevice::MapTextureExternalFormat(const TextureFormat format) {
        switch (format) {
            case TextureFormat::RGBA: return GL_RGBA;
            case TextureFormat::RGB: return GL_RGB;
            case TextureFormat::DEPTH: return GL_DEPTH_COMPONENT;
            case TextureFormat::SRGBA: return GL_SRGB_ALPHA;
        }
        return GL_RGBA;
    }

    GLenum OGLRenderDevice::MapFillMode(const FillMode mode) {
        switch (mode) {
            case FillMode::Solid: return GL_FILL;
            case FillMode::Wireframe: return GL_LINE;
        }
        return GL_NONE;
    }

    GLenum OGLRenderDevice::MapAttachment(const AttachmentType type) {
        switch (type) {
            case AttachmentType::COLOR: return GL_COLOR_ATTACHMENT0;
            case AttachmentType::DEPTH: return GL_DEPTH_ATTACHMENT;
            case AttachmentType::STENCIL: return GL_STENCIL_ATTACHMENT;
            case AttachmentType::NONE:
            default:
                return 0;
        }
    }
} // namespace Rendering
