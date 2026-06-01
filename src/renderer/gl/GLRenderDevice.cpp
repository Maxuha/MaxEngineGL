//
// Created by zykov on 5/1/2026.
//

#include "GLRenderDevice.h"
#include "../AmbientLightData.h"
#include "../domain/GLCommandBuffer.h"
#include "../domain/GLResourceSet.h"
#include "../domain/buffer/BufferDesc.h"
#include "../domain/struct/PipelineHandle.h"
#include "glm/gtc/type_ptr.hpp"
#include "../../math/Color.h"
#include "../domain/IWindowContext.h"


namespace Rendering {
    class GLTexture;
    struct GLCommand_BindPipeline;
    enum class GLCommandType : uint8_t;

    GLRenderDevice::GLRenderDevice(IWindowContext &context) : context(context),
                                                              window(
                                                                  static_cast<GLFWwindow *>(context.
                                                                      GetNativeHandle())) {
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

    TextureId GLRenderDevice::CreateTexture(const TextureCreateRequest &desc, const void *initialData) {
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
        } else {
            glTextureParameteri(glId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }
        glTextureParameteri(glId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        const auto id = TextureId{.Id = static_cast<uint32_t>(textures.size())};

        TextureData data;
        data.id = glId;
        textures.push_back(data);

        return id;
    }

    void GLRenderDevice::UpdateTextureFilterMode(TextureId &id, const TextureWrap textureWrap) {
        glTextureParameteri(id.Id, GL_TEXTURE_WRAP_T, textureWrapMap[textureWrap]);
        glTextureParameteri(id.Id, GL_TEXTURE_WRAP_S, textureWrapMap[textureWrap]);
        glTextureParameteri(id.Id, GL_TEXTURE_WRAP_R, textureWrapMap[textureWrap]);
    }

    IResourceSet *GLRenderDevice::CreateResourceSet() {
        auto *resourceSet = new GLResourceSet();
        return resourceSet;
    }

    BufferId GLRenderDevice::CreateBuffer(const BufferDesc &desc, const void *data) {
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

        const auto id = BufferId{.Id = static_cast<uint32_t>(buffers.size())};
        buffers.push_back(glId);
        return id;
    }

    void GLRenderDevice::UpdateBuffer(const BufferId id, void *data, const int offset, const int size) {
        // TODO Check if buffer is available for update

        glNamedBufferSubData(buffers[id.Id], static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size), data);
    }

    void GLRenderDevice::DestroyBuffer(const BufferId id) {
        glDeleteBuffers(1, &buffers[id.Id]);
    }

    void GLRenderDevice::Prepare() {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void GLRenderDevice::DrawIndexed(const size_t indexCount) {
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
    }

    void GLRenderDevice::BindResourceSet(const ResourceSet &resourceSet) {
        for (const auto &binding: resourceSet.bindings) {
            if (binding.type == ResourceType::SamplerTexture) {
                const GLuint glTexture = textures[binding.resourceId].id;

                glBindTextureUnit(binding.slot, glTexture);
            } else if (binding.type == ResourceType::UniformBuffer) {
                const GLuint glBuffer = buffers[binding.resourceId];

                glBindBufferBase(GL_UNIFORM_BUFFER, binding.slot, glBuffer);
            }
        }
    }

    void GLRenderDevice::BindPipeline(const ShaderId shaderId) {
        glUseProgram(shaders[shaderId.Id]);
        currentPipeline = shaders[shaderId.Id];
    }

    void GLRenderDevice::BindMesh(const MeshId meshId) {
        glBindVertexArray(meshIds[meshId]);
    }

    void GLRenderDevice::BindVertexBuffer(const BufferId id) {
        glVertexArrayVertexBuffer(currentPso->vao, 0, buffers[id.Id], 0, currentPso->vertexStrides[0]);
    }

    void GLRenderDevice::BindIndexBuffer(const BufferId id) {
        glVertexArrayElementBuffer(currentPso->vao, buffers[id.Id]);
    }

    void GLRenderDevice::BindUniformBuffer(const BufferId id, const uint32_t index) {
        glBindBufferBase(GL_UNIFORM_BUFFER, index, buffers[id.Id]);
    }

    void GLRenderDevice::PushConstants(const std::string key, Matrix4x4 mat) {
        // const GLint varId = glGetUniformLocation(1, key.c_str());
        // glUniformMatrix4fv(varId, 1, false, glm::value_ptr(mat.Convert<glm::mat4>()));

        glProgramUniformMatrix4fv(
            1, // TODO should be pipeline here
            3,
            1,
            GL_FALSE,
            glm::value_ptr(mat.Convert<glm::mat4>())
        );
    }

    void GLRenderDevice::PushConstants(const std::string &key, const int value) {
        const GLint varId = glGetUniformLocation(currentPipeline, key.c_str());
        glUniform1i(varId, value);
    }

    void GLRenderDevice::PushConstants(const std::string &key, const float value) {
        const GLint varId = glGetUniformLocation(currentPipeline, key.c_str());
        glUniform1f(varId, value);
    }

    void GLRenderDevice::PushConstants(const std::string &key, const ::Color value) {
        const GLint varId = glGetUniformLocation(currentPipeline, key.c_str());
        glUniform3f(varId, value.r, value.g, value.b);
    }

    void GLRenderDevice::PushConstants(const std::string &key, Vector3 value) {
        const GLint varId = glGetUniformLocation(currentPipeline, key.c_str());
        glUniform3fv(varId, 1, glm::value_ptr(value.Convert<glm::vec3>()));
    }


    void GLRenderDevice::DrawIndexed(const unsigned count, const MeshId vao) {
        glBindVertexArray(meshIds[vao]);
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
    }

    void GLRenderDevice::SetViewport(const Viewport viewport) {
        glViewport(static_cast<GLint>(viewport.x), static_cast<GLint>(viewport.y),
                   static_cast<GLsizei>(viewport.width), static_cast<GLsizei>(viewport.height));
    }

    void GLRenderDevice::SetCullMode(const CullMode mode) {
        if (mode == CullMode::None) {
            glDisable(GL_CULL_FACE);
        } else {
            glEnable(GL_CULL_FACE);
            glCullFace(mode == CullMode::Back ? GL_BACK : GL_FRONT);
        }
    }

    void GLRenderDevice::BeginRenderPass(ClearFlags flags, const ::Color color, const float depth) {
        GLbitfield glFlags = 0;
        if (static_cast<uint32_t>(flags) & static_cast<uint32_t>(ClearFlags::Color)) {
            glClearColor(color.r, color.g, color.b, color.a);
            glFlags |= GL_COLOR_BUFFER_BIT;
        }
        if (static_cast<uint32_t>(flags) & static_cast<uint32_t>(ClearFlags::Depth)) {
            glClearDepth(depth);
            glFlags |= GL_DEPTH_BUFFER_BIT;
        }
        if (glFlags != 0)
            glClear(glFlags);
    }

    PipelineHandle GLRenderDevice::CreatePipeline(const std::vector<ShaderInput> &shaders,
                                                  const RenderState &renderState) {
        IPipeline *newPipeline = new GLPipeline();

        newPipeline->Compile(shaders, renderState);

        const auto psoIndex = PipelineHandle{.Id = static_cast<uint32_t>(pipelines.size())};

        pipelines.push_back(newPipeline);

        return psoIndex;
    }

    PipelineHandle GLRenderDevice::CreatePipelineState(const PipelineStateDesc &pipeline) {
        Pipeline nvgPipeline;

        nvgPipeline.shaderId = pipeline.shaderId;

        glCreateVertexArrays(1, &nvgPipeline.vao);
        SetupVertexLayoutForPSO(nvgPipeline.vao, pipeline.vertexLayout);

        for (const auto &binding: pipeline.vertexLayout.bindings) {
            nvgPipeline.vertexStrides.push_back(binding.stride);
        }

        const uint32_t psoIndex = psoPool.size();
        psoPool.push_back(nvgPipeline);

        return PipelineHandle{.Id = psoIndex};
    }

    void GLRenderDevice::SetPipelineState(const PipelineHandle pipeline) {
        currentPso = &psoPool[pipeline.Id];
        currentPipeline = shaders[currentPso->shaderId.Id];
        glUseProgram(currentPipeline);
        glBindVertexArray(currentPso->vao);
    }

    TextureData GLRenderDevice::GetTextureData(const TextureId id) {
        return textures[id.Id];
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

                    glBindFramebuffer(GL_FRAMEBUFFER, cmd->fboId);
                    glViewport(0, 0, cmd->width, cmd->height);

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

                    const GLPipeline *pipeline = dynamic_cast<GLPipeline *>(pipelines[cmd->pipeline.Id]);
                    if (!pipeline)break;

                    glUseProgram(pipeline->GetProgram());

                    const RenderState &state = pipeline->GetRenderState();

                    break;
                }

                case GLCommandType::BindVertexBuffer: {
                    const auto *cmd = reinterpret_cast<GLCommand_BindBuffer *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_BindBuffer);

                    glVertexArrayVertexBuffer(commandBuffer->GetVao(), 0, buffers[cmd->buffer.Id], 0, sizeof(Vertex));
                    break;
                }

                case GLCommandType::BindIndexBuffer: {
                    const auto *cmd = reinterpret_cast<GLCommand_BindBuffer *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_BindBuffer);

                    glVertexArrayElementBuffer(commandBuffer->GetVao(), buffers[cmd->buffer.Id]);
                    break;
                }

                case GLCommandType::BindUniformBuffer: {
                    break;
                }

                case GLCommandType::BindResourceSet: {
                    const auto *cmd = reinterpret_cast<GLCommand_BindResourceSet *>(&commands[readOffset]);
                    readOffset += sizeof(GLCommand_BindResourceSet);

                    for (const auto &[bindingPoint, binding]: cmd->resourceSet->GetBindings()) {
                        if (binding.type == ResourceType::UniformBuffer) {
                            auto *glBuffer = static_cast<BufferId *>(binding.resourcePtr);
                            glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, buffers[glBuffer->Id]);
                        } else if (binding.type == ResourceType::SamplerTexture) {
                            auto *glTexture = static_cast<IGLTexture *>(binding.resourcePtr);
                            glBindTextureUnit(bindingPoint, textures[glTexture->GetId().Id].id);
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
                           1, // TODO should be pipeline here
                           1,
                           static_cast<GLint>(value1)
                       );
                    } else {
                        glProgramUniformMatrix4fv(
                          1, // TODO should be pipeline here
                          3,
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

                    glBindVertexArray(commandBuffer->GetVao());

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

            const GLenum glType = GL_FLOAT;
            const GLint components = MapGLComponents(attr.format);
            const GLboolean normalized = GL_FALSE;

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
} // namespace Rendering
