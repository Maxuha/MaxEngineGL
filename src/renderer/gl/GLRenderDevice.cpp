//
// Created by zykov on 5/1/2026.
//

#include "GLRenderDevice.h"
#include "../AmbientLightData.h"
#include "GLCommandBuffer.h"
#include "../domain/GLResourceSet.h"
#include "../domain/buffer/BufferDesc.h"
#include "../domain/struct/PipelineHandle.h"
#include "glm/gtc/type_ptr.hpp"
#include "../../math/Color.h"
#include "../domain/IWindowContext.h"
#include "model/PipelineData.h"


namespace Rendering {
    class GLTexture;
    struct GLCommand_BindPipeline;
    enum class GLCommandType : uint8_t;

    GLRenderDevice::GLRenderDevice(IWindowContext &context) : context(context),
                                                              window(
                                                                  static_cast<GLFWwindow *>(context.
                                                                      GetNativeHandle())) {
        bufferManager = new BufferManager();
    }

    GLRenderDevice::~GLRenderDevice() = default;

    ShaderId GLRenderDevice::CreateShader(const char *vCode, const char *fCode) {
        GLint success;
        GLchar infoLog[1024];

        // 1. Vertex shader
        const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vCode, nullptr);
        glCompileShader(vertexShader);

        // ???????? ??????????? ???????
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertexShader, 1024, nullptr, infoLog);
            printf("??????????? ?????? ?????????? VERTEX ???????:\n%s\n", infoLog);
#ifdef _MSC_VER
            __debugbreak();
#endif
        }

        // 2. Fragment shader
        const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fCode, nullptr);
        glCompileShader(fragmentShader);

        // ???????? ???????????? ???????
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragmentShader, 1024, nullptr, infoLog);
            printf("??????????? ?????? ?????????? FRAGMENT ???????:\n%s\n", infoLog);
#ifdef _MSC_VER
            __debugbreak();
#endif
        }

        // 3. ?????? ?????????
        const GLuint glId = glCreateProgram();
        glAttachShader(glId, vertexShader);
        glAttachShader(glId, fragmentShader);
        glLinkProgram(glId);

        glGetProgramiv(glId, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(glId, 1024, nullptr, infoLog);
            printf("?????? ???????? ?????????:\n%s\n", infoLog);
#ifdef _MSC_VER
            __debugbreak();
#endif
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        const ShaderId id{.Id = static_cast<uint32_t>(shaders.size())};
        shaders.push_back(glId);
        return id;
    }

    TextureHandle GLRenderDevice::CreateTexture(const TextureCreateRequest &desc, const void *initialData) {
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

    void GLRenderDevice::UpdateTextureFilterMode(TextureHandle &id, const TextureWrap textureWrap) {
        glTextureParameteri(id.Id, GL_TEXTURE_WRAP_T, textureWrapMap[textureWrap]);
        glTextureParameteri(id.Id, GL_TEXTURE_WRAP_S, textureWrapMap[textureWrap]);
        glTextureParameteri(id.Id, GL_TEXTURE_WRAP_R, textureWrapMap[textureWrap]);
    }

    IResourceSet *GLRenderDevice::CreateResourceSet() {
        auto *resourceSet = new GLResourceSet();
        return resourceSet;
    }

    BufferHandle GLRenderDevice::CreateBuffer(const BufferDesc &desc, const void *data) {
        GLuint glId;
        glCreateBuffers(1, &glId);

        GLbitfield flags = 0;

        if (desc.usage == BufferUsage::Uniform) {
            flags |= GL_DYNAMIC_STORAGE_BIT;
        }

        glNamedBufferStorage(
            glId,
            static_cast<GLsizeiptr>(desc.size),
            data,
            flags
        );

        const auto id = BufferHandle{.Id = static_cast<uint32_t>(buffers.size())};
        buffers.push_back(glId);
        return id;
    }

    BufferHandle GLRenderDevice::CreateFrameBuffer(const TextureHandle texture) {
        return bufferManager->CreateFrameBuffer(textures[texture.Id]);

        // GLuint fbo;
        // glCreateFramebuffers(1, &fbo);
        //
        // glNamedFramebufferTexture(fbo, GL_DEPTH_ATTACHMENT, textures[texture.Id].id, 0);
        //
        // glNamedFramebufferDrawBuffer(fbo, GL_NONE);
        // glNamedFramebufferReadBuffer(fbo, GL_NONE);
        //
        // const GLenum status = glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER);
        // if (status != GL_FRAMEBUFFER_COMPLETE) {
        //     std::cout << "FrameBuffer is not complete: " << status << std::endl;
        // }
        //
        // const auto id = BufferHandle{.Id = static_cast<uint32_t>(buffers.size())};
        // buffers.push_back(fbo);
        // return id;
    }

    void GLRenderDevice::UpdateBuffer(const BufferHandle id, void *data, const int offset, const int size) {
        // TODO Check if buffer is available for update
        glNamedBufferSubData(buffers[id.Id], static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size), data);
    }

    void GLRenderDevice::DestroyBuffer(const BufferHandle id) {
        glDeleteBuffers(1, &buffers[id.Id]);
    }

    void GLRenderDevice::BindUniformBuffer(const BufferHandle id, const uint32_t index) {
        glBindBufferBase(GL_UNIFORM_BUFFER, index, buffers[id.Id]);
    }

    PipelineHandle GLRenderDevice::CreatePipeline(const std::vector<ShaderInput> &shaders,
                                                  const PipelineStateDesc &pipelineStateDesc) {
        auto *newPipeline = new GLPipeline(pipelineStateDesc);

        newPipeline->Compile(shaders);

        GLuint vao;

        glCreateVertexArrays(1, &vao);

        SetupVertexLayoutForPSO(vao, pipelineStateDesc.vertexLayout);

        PipelineData pipelineData{};
        pipelineData.vao = vao;
        pipelineData.program = newPipeline->GetProgram();
        pipelineData.depthTestEnable = pipelineStateDesc.depthStencilState.depthTestEnable;
        pipelineData.depthWriteEnable = pipelineStateDesc.depthStencilState.depthWriteEnable;
        pipelineData.cullEnable = pipelineStateDesc.rasterizerState.cullEnable;
        pipelineData.cullMode = MapCullMode(pipelineStateDesc.rasterizerState.cullMode);
        pipelineData.fillMode = MapFillMode(pipelineStateDesc.rasterizerState.fillMode);

        const auto psoIndex = PipelineHandle{.Id = static_cast<uint32_t>(pipelines.size())};

        pipelines.push_back(pipelineData);

        return psoIndex;
    }

    void GLRenderDevice::SubmitCommandBuffer(ICommandBuffer *commandBuffer) {
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

                    float width;
                    float height;
                    GLuint fbo;
                    if (cmd->fboId == -1) {
                        fbo = 0;
                        width = static_cast<float>(cmd->width);
                        height = static_cast<float>(cmd->height);
                    } else {
                        fbo = buffers[cmd->fboId];
                        width = static_cast<float>(2048);
                        height = static_cast<float>(2048);
                    }

                    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
                    glViewport(0, 0, width, height);

                    GLbitfield clearMask = 0;

                    if (cmd->attachment.loadOp == AttachmentLoadOp::CLEAR) {
                        if ((cmd->attachment.type & AttachmentType::COLOR) != AttachmentType::NONE) {
                            clearMask |= GL_COLOR_BUFFER_BIT;
                        }
                        if ((cmd->attachment.type & AttachmentType::DEPTH) != AttachmentType::NONE) {
                            clearMask |= GL_DEPTH_BUFFER_BIT;
                        }
                        if ((cmd->attachment.type & AttachmentType::STENCIL) != AttachmentType::NONE) {
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

                    if (cmd->attachment.storeOp == AttachmentStoreOp::DONT_CARE) {
                        GLenum attachment = (cmd->attachment.type == AttachmentType::DEPTH)
                                                ? GL_DEPTH_ATTACHMENT
                                                : GL_COLOR_ATTACHMENT0;

                        //glInvalidateFramebuffer(GL_FRAMEBUFFER, static_cast<GLsizei>(1), &attachment);
                    }

                    glBindFramebuffer(GL_FRAMEBUFFER, 0);

                    break;
                }

                case GLCommandType::BindPipeline: {
                    const auto *cmd = reinterpret_cast<GLCommand_BindPipeline *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_BindPipeline);

                    currentPipeline = cmd->pipeline;

                    const auto pipeline = pipelines[currentPipeline.Id];

                    glUseProgram(pipeline.program);

                    pipeline.cullEnable ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
                    pipeline.depthTestEnable ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);

                    glDepthMask(pipeline.depthWriteEnable);

                    glCullFace(pipeline.cullMode);

                    //TODO check this
                    glPolygonMode(GL_FRONT_AND_BACK, pipeline.fillMode);

                    break;
                }

                case GLCommandType::BindVertexBuffer: {
                    const auto *cmd = reinterpret_cast<GLCommand_BindBuffer *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_BindBuffer);

                    glVertexArrayVertexBuffer(pipelines[currentPipeline.Id].vao, 0,
                                              buffers[cmd->buffer.Id], 0, sizeof(Vertex));
                    break;
                }

                case GLCommandType::BindIndexBuffer: {
                    const auto *cmd = reinterpret_cast<GLCommand_BindBuffer *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_BindBuffer);

                    glVertexArrayElementBuffer(pipelines[currentPipeline.Id].vao, buffers[cmd->buffer.Id]);
                    break;
                }

                case GLCommandType::BindUniformBuffer: {
                    const auto *cmd = reinterpret_cast<GLCommand_BindUniform *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_BindUniform);

                    glBindBufferBase(GL_UNIFORM_BUFFER, cmd->bindingPoint, buffers[cmd->buffer.Id]);
                    break;
                }

                case GLCommandType::UpdateBuffer: {
                    const auto *cmd = reinterpret_cast<GLCommand_UpdateBuffer *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_UpdateBuffer);

                    glNamedBufferSubData(buffers[cmd->buffer.Id], static_cast<GLintptr>(cmd->offset),
                                         static_cast<GLsizeiptr>(cmd->size), cmd->data);
                    break;
                }

                case GLCommandType::BindResourceSet: {
                    const auto *cmd = reinterpret_cast<GLCommand_BindResourceSet *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_BindResourceSet);

                    for (const auto &[bindingPoint, binding]: cmd->resourceSet->GetBindings()) {
                        if (binding.type == ResourceType::UniformBuffer) {
                            const auto *glBuffer = static_cast<BufferHandle *>(binding.resourcePtr);
                            glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, buffers[glBuffer->Id]);
                        } else if (binding.type == ResourceType::SamplerTexture) {
                            glBindTextureUnit(bindingPoint, textures[binding.texture.Id].id);
                        }
                    }
                    break;
                }

                case GLCommandType::PushConstants: {
                    const auto *cmd = reinterpret_cast<GLCommand_PushConstants *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_PushConstants);

                    auto value = cmd->mat;
                    const auto value1 = cmd->value;

                    if (value1) {
                        glProgramUniform1i(
                            pipelines[currentPipeline.Id].program, // TODO should be pipeline here
                            1,
                            static_cast<GLint>(value1)
                        );
                    } else {
                        glProgramUniformMatrix4fv(
                             pipelines[currentPipeline.Id].program, // TODO should be pipeline here
                             //pipelines[currentPipeline.Id].layout.items[cmd->key].bindingIndex,
                             cmd->key,
                             1,
                             GL_FALSE,
                             glm::value_ptr(value.Convert<glm::mat4>())
                         );
                    }

                    break;
                }

                case GLCommandType::Draw: {
                    const auto *cmd = reinterpret_cast<GLCommand_Draw *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_Draw);

                    glBindVertexArray(pipelines[currentPipeline.Id].vao);

                    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(cmd->vertexCount), GL_UNSIGNED_INT, nullptr);

                    glBindVertexArray(0);
                    break;
                }

                default:
                    break;
            }
        }

        head = 0;
    }

    void GLRenderDevice::Present() {
        glfwSwapBuffers(window);
    }

    void GLRenderDevice::SetupVertexLayoutForPSO(const GLuint vao, const VertexLayout &layout) {
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

    GLint GLRenderDevice::MapGLComponents(const VertexFormat format) {
        switch (format) {
            case VertexFormat::Float3: return 3;
            case VertexFormat::Float4: return 4;
            case VertexFormat::Float2: return 2;
            case VertexFormat::Int4: return 4;
            case VertexFormat::UByte4N: return 4;
            default: return 0;
        }
    }

    GLenum GLRenderDevice::MapTextureInternalFormat(const TextureFormat format) {
        switch (format) {
            case TextureFormat::RGBA: return GL_RGBA8;
            case TextureFormat::RGB: return GL_RGB8;
            case TextureFormat::D32_FLOAT: return GL_DEPTH_COMPONENT32F;
            case TextureFormat::DEPTH: return GL_DEPTH_COMPONENT24;
        }
        return GL_RGBA8;
    }

    GLenum GLRenderDevice::MapTextureExternalFormat(const TextureFormat format) {
        switch (format) {
            case TextureFormat::RGBA: return GL_RGBA;
            case TextureFormat::RGB: return GL_RGB;
            case TextureFormat::DEPTH: return GL_DEPTH_COMPONENT;
            case TextureFormat::D32_FLOAT: return GL_DEPTH_COMPONENT;
        }
        return GL_RGBA;
    }

    GLenum GLRenderDevice::MapCullMode(const CullMode mode) {
        switch (mode) {
            case CullMode::Front: return GL_FRONT;
            case CullMode::Back: return GL_BACK;
        }
        return GL_NONE;
    }

    GLenum GLRenderDevice::MapFillMode(const FillMode mode) {
        switch (mode) {
            case FillMode::Solid: return GL_FILL;
            case FillMode::Wireframe: return GL_LINE;
        }
        return GL_NONE;
    }
} // namespace Rendering
