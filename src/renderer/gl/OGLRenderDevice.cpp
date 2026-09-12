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
#include "../../math/Color.h"


namespace Rendering {
    OGLRenderDevice::OGLRenderDevice(IWindow& window) {
        this->window = static_cast<GLFWwindow *>(window.GetHandle());

        instance = new OGLInstance(window);

        textureManager = new OGLTextureManager();
        pipelineManager = new OGLPipelineManager();
        shaderManager = new OGLShaderManager();
        bufferManager = new OGLVertexBufferManager();
        commandPoolManager = new OGLCommandPoolManager();
        renderPassManager = new OGLRenderPassManager();
        descriptorSetManager = new OGLDescriptorSetManager();
    }

    OGLRenderDevice::~OGLRenderDevice() = default;

    CommandBuffer * OGLRenderDevice::AllocateCommandBuffer() {
        return new CommandBuffer;
    }

    TextureHandle OGLRenderDevice::CreateTexture(const TextureCreateRequest &desc, const void *initialData) {
        const auto _data = const_cast<void *>(initialData);

        return textureManager->CreateTexture(desc.Width, desc.Height, desc.Format, _data);

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

            constexpr float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
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

    ResourceSetLayoutHandle OGLRenderDevice::CreateResourceSetLayout(const ResourceSetLayoutDesc &resourceSetLayoutDesc) {
        return {};
    }

    ResourceSetHandle OGLRenderDevice::CreateResourceSet(const PipelineHandle pipelineHandle, const ResourceBinding resource) {
        const OGLPipeline pipeline = pipelineManager->GetPipeline(PipelineHandle{.Id = 0});

        if (resource.resourceId == 999999) {
            return descriptorSetManager->CreateDescriptorSet(pipeline.layout.resources[0]);
        }
        if (resource.resourceId == 999998) {
            return descriptorSetManager->CreateDescriptorSet(pipeline.layout.resources[1]);
        }

        return descriptorSetManager->CreateDescriptorSet(pipeline.layout.resources[2]);
    }

    ResourceSetHandle OGLRenderDevice::CreateResourceSet(const ResourceSetLayoutHandle resourceSetLayoutHandle) {
        return {};
    }

    void OGLRenderDevice::UpdateResourceSet(const ResourceSetHandle setHandle, const uint32_t binding, const TextureHandle textureHandle) {
        descriptorSetManager->BindTexture(setHandle, binding, textureHandle);
    }

    void OGLRenderDevice::UpdateResourceSet(const ResourceSetHandle setHandle, const uint32_t binding, const BufferHandle bufferHandle) {
        descriptorSetManager->BindBuffer(setHandle, binding, bufferHandle);
    }

    BufferHandle OGLRenderDevice::CreateBuffer(const BufferDesc &desc, const void *data) {
        return bufferManager->CreateBuffer(desc, data);
    }

    BufferHandle OGLRenderDevice::CreateFrameBuffer(const TextureHandle texture) {
        // return bufferManager->CreateFrameBuffer(textures[texture.Id]);
        return {};
    }

    void OGLRenderDevice::UpdateBuffer(const BufferHandle id, const void *data, const int offset, const int size) {
        // TODO Check if buffer is available for update
       // glNamedBufferSubData(buffers[id.Id], static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size), data);

        bufferManager->UpdateUniformBuffer(id, data, offset, size);
    }

    void OGLRenderDevice::DestroyBuffer(const BufferHandle id) {
        glDeleteBuffers(1, &buffers[id.Id]);
    }

    void OGLRenderDevice::BindUniformBuffer(const BufferHandle id, const uint32_t index) {
        glBindBufferBase(GL_UNIFORM_BUFFER, index, buffers[id.Id]);
    }

    PipelineHandle OGLRenderDevice::CreatePipeline(const std::vector<ShaderHandle> &shaders,
                                                         const RenderPassHandle renderPass,
                                                         const PipelineStateDesc &pipelineStateDesc) {

        const PipelineHandle pipelineHandle = pipelineManager->CreatePipeline(shaders, renderPass, pipelineStateDesc);
        const OGLPipeline pipeline = pipelineManager->GetPipeline(pipelineHandle);

        for (const auto shaderHandle : shaders) {
            OGLShader shader = shaderManager->GetShader(shaderHandle);
            glAttachShader(pipeline.program, shader.handle);
        }

        glLinkProgram(pipeline.program);

        for (const auto shaderHandle : shaders) {
            OGLShader shader = shaderManager->GetShader(shaderHandle);

            glDetachShader(pipeline.program, shader.handle);
            glDeleteShader(shader.handle);
        }

        return pipelineHandle;
    }

    PipelineHandle OGLRenderDevice::CreatePipeline(const ShaderHandle shaderHandle, const PipelineStateDesc &pipelineStateDesc) {

        const OGLGraphicsShader shader = _graphicsShaders[shaderHandle.Id];

        const PipelineHandle pipelineHandle = pipelineManager->CreatePipeline(shader, pipelineStateDesc);

        return pipelineHandle;

    }

    PipelineHandle OGLRenderDevice::CreatePipeline(const ShaderDesc &shaderDesc, const PipelineStateDesc &pipelineStateDesc) {

        const ShaderHandle vShaderHandle = shaderManager->CreateShader(ShaderType::Vertex, shaderDesc.vertexCode);
        const ShaderHandle fShaderHandle = shaderManager->CreateShader(ShaderType::Fragment, shaderDesc.fragmentCode);

        OGLGraphicsShader shader;
        shader.vShader = shaderManager->GetShader(vShaderHandle).handle;
        shader.fShader = shaderManager->GetShader(fShaderHandle).handle;

        for (auto resource_set_layout : shaderDesc.resourceSetLayouts) {
            OGLDescriptorSetLayout layout;
            layout.set_index = resource_set_layout.set_index;
            for (const auto binding : resource_set_layout.bindings) {
                OGLDescriptorSetLayoutItem item;
                item.binding_slot = binding.binding_slot;
                item.stage_flags = binding.stage_flags;
                item.type = binding.type;
                layout.bindings.push_back(item);
            }
            shader.layouts.push_back(layout);
        }

        _graphicsShaders.push_back(shader);

        const auto shaderHandle = ShaderHandle { .Id = static_cast<uint32_t>(_graphicsShaders.size() - 1) };

        return CreatePipeline(shaderHandle, pipelineStateDesc);
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

                   // const auto renderPass = static_cast<OGLRenderPassManager *>(renderContext.RenderPassManager)->GetRenderPass(cmd->renderPass);
                  //  const auto framebuffer = static_cast<OGLFrameBufferManager *>(renderContext.FrameBufferManager)->GetFrameBuffer(cmd->frameBuffer);

                    const float width = cmd->width;
                    const float height = cmd->height;
                    const GLuint fbo = 0;

                    // if (fbo == -1) {
                    //     fbo = 0;
                    //     width = static_cast<float>(cmd->width);
                    //     height = static_cast<float>(cmd->height);
                    // } else {
                    //     fbo = buffers[cmd->fboId];
                    //     width = static_cast<float>(2048);
                    //     height = static_cast<float>(2048);
                    // }

                    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
                    glViewport(0, 0, width, height);

                    GLbitfield clearMask = 0;

                    if (cmd->attachment1.loadOp == AttachmentLoadOp::CLEAR) {
                        if (cmd->attachment1.type == AttachmentType::COLOR) {
                            clearMask |= GL_COLOR_BUFFER_BIT;
                        }
                        if (cmd->attachment1.type == AttachmentType::DEPTH) {
                            clearMask |= GL_DEPTH_BUFFER_BIT;
                        }
                        if (cmd->attachment1.type == AttachmentType::STENCIL) {
                            clearMask |= GL_STENCIL_BUFFER_BIT;
                        }
                    }

                    if (cmd->attachment2.loadOp == AttachmentLoadOp::CLEAR) {
                        if (cmd->attachment2.type == AttachmentType::COLOR) {
                            clearMask |= GL_COLOR_BUFFER_BIT;
                        }
                        if (cmd->attachment2.type == AttachmentType::DEPTH) {
                            clearMask |= GL_DEPTH_BUFFER_BIT;
                        }
                        if (cmd->attachment2.type == AttachmentType::STENCIL) {
                            clearMask |= GL_STENCIL_BUFFER_BIT;
                        }
                    }

                    if (clearMask != 0) {
                        glClearColor(cmd->clearColor.r, cmd->clearColor.g, cmd->clearColor.b, cmd->clearColor.a);
                        glClear(clearMask);
                    }
                    break;
                }

                case GLCommandType::EndRenderPass: {
                    const auto *cmd = reinterpret_cast<GLCommand_EndRenderPass *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_EndRenderPass);

                    // if (cmd->attachment.storeOp == AttachmentStoreOp::DONT_CARE) {
                    //     GLenum attachment = (cmd->attachment.type == AttachmentType::DEPTH)
                    //                             ? GL_DEPTH_ATTACHMENT
                    //                             : GL_COLOR_ATTACHMENT0;
                    //
                    //     //glInvalidateFramebuffer(GL_FRAMEBUFFER, static_cast<GLsizei>(1), &attachment);
                    // }
                    //
                    // glBindFramebuffer(GL_FRAMEBUFFER, 0);

                    break;
                }

                case GLCommandType::BindPipeline: {
                    const auto *cmd = reinterpret_cast<GLCommand_BindPipeline *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_BindPipeline);

                    currentPipeline = cmd->pipeline;

                    const auto pipeline = pipelineManager->GetPipeline(currentPipeline);

                    glUseProgram(pipeline.program);

                    // glDisable(GL_CULL_FACE);
                     // glDisable(GL_DEPTH_TEST);
                    //  pipeline.cullEnable ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
                    pipeline.depthTestEnable ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
                    //
                    //  glDepthMask(pipeline.depthWriteEnable);
                    // //
                    //  glCullFace(pipeline.cullMode);
                    // //
                    //  // TODO check this
                    //  glPolygonMode(GL_FRONT_AND_BACK, pipeline.fillMode);

                    break;
                }

                case GLCommandType::BindVertexBuffer: {
                    const auto *cmd = reinterpret_cast<GLCommand_BindBuffer *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_BindBuffer);

                    const auto pipeline = pipelineManager->GetPipeline(currentPipeline);
                    const auto buffer = bufferManager->GetBuffer(cmd->buffer);

                    glVertexArrayVertexBuffer(pipeline.vao, 0,
                                              buffer.glId, 0, sizeof(Vertex));
                    break;
                }

                case GLCommandType::BindIndexBuffer: {
                    const auto *cmd = reinterpret_cast<GLCommand_BindBuffer *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_BindBuffer);

                    const auto pipeline = pipelineManager->GetPipeline(currentPipeline);
                    const auto buffer = bufferManager->GetBuffer(cmd->buffer);

                    glVertexArrayElementBuffer(pipeline.vao, buffer.glId);
                    break;
                }

                case GLCommandType::UpdateBuffer: {
                    const auto *cmd = reinterpret_cast<GLCommand_UpdateBuffer *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_UpdateBuffer);

                    const auto bufferHandle = cmd->buffer;
                    const auto data = cmd->data;
                    const auto offset = cmd->offset;
                    const auto size = cmd->size;

                    bufferManager->UpdateUniformBuffer(bufferHandle, data, offset, size);
                    break;
                }

                case GLCommandType::UpdateUniformBuffer: {
                    const auto *cmd = reinterpret_cast<GLCommand_UpdateBuffer *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_UpdateBuffer);

                    const auto bufferHandle = cmd->buffer;
                    const auto data = cmd->data;
                    const auto offset = cmd->offset;
                    const auto size = cmd->size;

                    OGLBuffer buffer = bufferManager->GetBuffer(bufferHandle);

                    glNamedBufferSubData(buffer.glId, static_cast<GLintptr>(offset),
                                         static_cast<GLsizeiptr>(size), data);
                    break;
                }

                case GLCommandType::BindResourceSet: {
                    const auto *cmd = reinterpret_cast<GLCommand_BindResourceSet *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_BindResourceSet);

                    OGLResourceSet resourceSet = descriptorSetManager->GetResourceSet(cmd->resourceId);

                    for (auto item : resourceSet.items) {
                        //TODO Remove it
                        if (item.bufferHandle.Id == 9999999 && item.textureHandle.Id == 9999999) continue;

                        if (item.IsBuffer()) {
                            OGLBuffer buffer = bufferManager->GetBuffer(item.bufferHandle);
                            glBindBufferBase(GL_UNIFORM_BUFFER, item.binding, buffer.glId);
                        } else {
                            OGLTexture texture = textureManager->GetTexture(item.textureHandle);
                            glActiveTexture(GL_TEXTURE0 + item.binding);
                            glBindTexture(GL_TEXTURE_2D, texture.Id);
                        }
                    }
                    break;
                }

                case GLCommandType::PushConstants: {
                    const auto *cmd = reinterpret_cast<GLCommand_PushConstants *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_PushConstants);

                    auto value = cmd->mat;
                    const auto value1 = cmd->value;

                    // if (value1) {
                    //     glProgramUniform1i(
                    //         pipelines[currentPipeline.Id].program, // TODO should be pipeline here
                    //         1,
                    //         static_cast<GLint>(value1)
                    //     );
                    // } else {
                    //     glProgramUniformMatrix4fv(
                    //          pipelines[currentPipeline.Id].program, // TODO should be pipeline here
                    //          //pipelines[currentPipeline.Id].layout.items[cmd->key].bindingIndex,
                    //          cmd->key,
                    //          1,
                    //          GL_FALSE,
                    //          glm::value_ptr(value.Convert<glm::mat4>())
                    //      );
                    // }

                    break;
                }

                case GLCommandType::Draw: {
                    const auto *cmd = reinterpret_cast<GLCommand_Draw *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_Draw);

                    glDrawArrays(GL_TRIANGLES, 0, cmd->vertexCount);
                    break;
                }

                case GLCommandType::DrawIndexed: {
                    const auto *cmd = reinterpret_cast<GLCommand_Draw *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_Draw);

                    const auto pipeline = pipelineManager->GetPipeline(currentPipeline);

                    glBindVertexArray(pipeline.vao);

                    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(cmd->vertexCount), GL_UNSIGNED_SHORT, nullptr);

                    glBindVertexArray(0);
                    break;
                }

                default:
                    break;
            }
        }

        head = 0;
        Present();
    }

    void OGLRenderDevice::Present() {
        glfwSwapBuffers(window);
    }

    ImageFormat OGLRenderDevice::GetDisplayFormat() {
        return ImageFormat::RGBA8_Srgb;
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
            case TextureFormat::D32_FLOAT: return GL_DEPTH_COMPONENT32F;
            case TextureFormat::DEPTH: return GL_DEPTH_COMPONENT24;
        }
        return GL_RGBA8;
    }

    GLenum OGLRenderDevice::MapTextureExternalFormat(const TextureFormat format) {
        switch (format) {
            case TextureFormat::RGBA: return GL_RGBA;
            case TextureFormat::RGB: return GL_RGB;
            case TextureFormat::DEPTH: return GL_DEPTH_COMPONENT;
            case TextureFormat::D32_FLOAT: return GL_DEPTH_COMPONENT;
        }
        return GL_RGBA;
    }

    GLenum OGLRenderDevice::MapCullMode(const CullMode mode) {
        switch (mode) {
            case CullMode::Front: return GL_FRONT;
            case CullMode::Back: return GL_BACK;
        }
        return GL_NONE;
    }

    GLenum OGLRenderDevice::MapFillMode(const FillMode mode) {
        switch (mode) {
            case FillMode::Solid: return GL_FILL;
            case FillMode::Wireframe: return GL_LINE;
        }
        return GL_NONE;
    }
} // namespace Rendering
