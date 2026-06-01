//
// Created by zykov on 5/22/2026.
//

#include "GLCommandBuffer.h"

#include "GLPipeline.h"
#include "GLTexture.h"
#include "../../math/Vertex.h"
#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"
#include "../../math/Color.h"

Rendering::GLCommandBuffer::GLCommandBuffer(const size_t bufferSize) {
    buffer.resize(bufferSize);

    glCreateVertexArrays(1, &vao);

    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);

    glEnableVertexArrayAttrib(vao, 1);
    glVertexArrayAttribBinding(vao, 1, 0);
    glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));

    glEnableVertexArrayAttrib(vao, 2);
    glVertexArrayAttribBinding(vao, 2, 0);
    glVertexArrayAttribFormat(vao, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texCoords));
}

Rendering::GLCommandBuffer::~GLCommandBuffer() {
    if (vao != 0) {
        glDeleteVertexArrays(1, &vao);
    }
};

void Rendering::GLCommandBuffer::Begin() {
    head = 0;
}

void Rendering::GLCommandBuffer::End() {
}

void Rendering::GLCommandBuffer::Draw(const uint32_t vertexCount) {
    auto *cmd = allocateCommand<GLCommand_Draw>(GLCommandType::Draw);
    cmd->vertexCount = vertexCount;
}

void Rendering::GLCommandBuffer::BeginRenderPass(const AttachmentDescription& attachment,
                                                 const GLuint fboId, const int w, const int h,
                                                 const ::Color clearColor) {
    auto *cmd = allocateCommand<GLCommand_BeginRenderPass>(GLCommandType::BeginRenderPass);
    cmd->attachment = attachment;
    cmd->fboId = fboId;
    cmd->width = w;
    cmd->height = h;
    cmd->clearColor = clearColor;
}

void Rendering::GLCommandBuffer::EndRenderPass(const AttachmentDescription& attachment) {
    auto *cmd = allocateCommand<GLCommand_EndRenderPass>(GLCommandType::EndRenderPass);
    cmd->attachment = attachment;
}

void Rendering::GLCommandBuffer::BindPipeline(const PipelineHandle &pipeline) {
    auto *cmd = allocateCommand<GLCommand_BindPipeline>(GLCommandType::BindPipeline);

    //std::cout << &pipeline << std::endl;

    // std::cout << dynamic_cast<const GLPipeline *>(&pipeline) << std::endl;

    cmd->pipeline = pipeline;
}

void Rendering::GLCommandBuffer::BindVertexBuffer(BufferId &buffer) {
    auto *cmd = allocateCommand<GLCommand_BindBuffer>(GLCommandType::BindVertexBuffer);
    cmd->buffer = buffer;
}

void Rendering::GLCommandBuffer::BindIndexBuffer(BufferId &buffer) {
    auto *cmd = allocateCommand<GLCommand_BindBuffer>(GLCommandType::BindIndexBuffer);
    cmd->buffer = buffer;
}

void Rendering::GLCommandBuffer::BindUniformBuffer(const uint32_t bindingPoint, const BufferId &buffer) {
    auto *cmd = allocateCommand<GLCommand_BindUniform>(GLCommandType::BindUniformBuffer);
    cmd->bindingPoint = bindingPoint;
    cmd->buffer = buffer;
}

void Rendering::GLCommandBuffer::BindResourceSet(uint32_t setIndex, const IResourceSet &resourceSet) {
    auto *cmd = allocateCommand<GLCommand_BindResourceSet>(GLCommandType::BindResourceSet);
    cmd->resourceSet = &dynamic_cast< const GLResourceSet& >(resourceSet);
}

void Rendering::GLCommandBuffer::PushConstants(const std::string &key, const Matrix4x4 mat) {
    auto *cmd = allocateCommand<GLCommand_PushConstants>(GLCommandType::PushConstants);
    cmd->mat = mat;
}

void Rendering::GLCommandBuffer::PushConstants(const std::string &key, uint32_t value) {
    auto *cmd = allocateCommand<GLCommand_PushConstants>(GLCommandType::PushConstants);
    cmd->value = value;
}

std::vector<uint8_t>& Rendering::GLCommandBuffer::GetCommands() {
    return buffer;
}

size_t Rendering::GLCommandBuffer::GetHead() const {
    return head;
}

uint32_t Rendering::GLCommandBuffer::GetVao() const {
    return vao;
}

void Rendering::GLCommandBuffer::ExecuteAll() {
    //   size_t readOffset = 0;
    //
    // auto mapBlendFactor = [](const BlendFactor factor) -> GLenum {
    //     switch (factor) {
    //         case BlendFactor::One:              return GL_ONE;
    //         case BlendFactor::SrcAlpha:         return GL_SRC_ALPHA;
    //         case BlendFactor::OneMinusSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;
    //         case BlendFactor::Zero:
    //         default:                            return GL_ZERO;
    //     }
    // };
    //
    // while (readOffset < head) {
    //     const GLCommandType type = *reinterpret_cast<GLCommandType*>(&buffer[readOffset]);
    //     readOffset += sizeof(GLCommandType);
    //
    //     switch (type) {
    //         case GLCommandType::BindPipeline: {
    //             const auto* cmd = reinterpret_cast<GLCommand_BindPipeline*>(&buffer[readOffset]);
    //             readOffset += sizeof(GLCommand_BindPipeline);
    //
    //             const GLPipeline* pipeline = cmd->pipeline;
    //             if (!pipeline) break;
    //
    //             glUseProgram(pipeline->GetProgram());
    //
    //             const RenderState& state = pipeline->GetRenderState();
    //
    //             // if (state.depthTestEnable) {
    //             //     glEnable(GL_DEPTH_TEST);
    //             //     glDepthMask(state.depthWriteEnable ? GL_TRUE : GL_FALSE);
    //             // } else {
    //             //     glDisable(GL_DEPTH_TEST);
    //             // }
    //             //
    //             // if (state.cullMode == CullMode::None) {
    //             //     glDisable(GL_CULL_FACE);
    //             // } else {
    //             //     glEnable(GL_CULL_FACE);
    //             //     glCullFace(state.cullMode == CullMode::Front ? GL_FRONT : GL_BACK);
    //             //     glFrontFace(state.frontFaceClockwise ? GL_CW : GL_CCW);
    //             // }
    //             //
    //             // if (state.blendEnable) {
    //             //     glEnable(GL_BLEND);
    //             //     glBlendFunc(
    //             //         mapBlendFactor(state.srcColorFactor),
    //             //         mapBlendFactor(state.dstColorFactor)
    //             //     );
    //             // } else {
    //             //     glDisable(GL_BLEND);
    //             // }
    //             //
    //             // glPolygonMode(GL_FRONT_AND_BACK, state.wireframe ? GL_LINE : GL_FILL);
    //
    //             break;
    //         }
    //
    //         case GLCommandType::BindVertexBuffer: {
    //             const auto* cmd = reinterpret_cast<GLCommand_BindBuffer*>(&buffer[readOffset]);
    //             readOffset += sizeof(GLCommand_BindBuffer);
    //
    //             glVertexArrayVertexBuffer(vao, 0, cmd->buffer->GetId().Id, 0, sizeof(Vertex));
    //             break;
    //         }
    //
    //         case GLCommandType::BindIndexBuffer: {
    //             const auto* cmd = reinterpret_cast<GLCommand_BindBuffer*>(&buffer[readOffset]);
    //             readOffset += sizeof(GLCommand_BindBuffer);
    //
    //             glVertexArrayElementBuffer(vao, cmd->buffer->GetId().Id);
    //             break;
    //         }
    //
    //         case GLCommandType::BindUniformBuffer: {
    //
    //         }
    //
    //         case GLCommandType::BindResourceSet: {
    //             const auto* cmd = reinterpret_cast<GLCommand_BindResourceSet*>(&buffer[readOffset]);
    //             readOffset += sizeof(GLCommand_BindBuffer);
    //
    //             for (const auto& [bindingPoint, binding] : cmd->resourceSet->GetBindings()) {
    //                 if (binding.type == ResourceType::UniformBuffer) {
    //                     auto* glBuffer = static_cast<Buffer*>(binding.resourcePtr);
    //                     glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, glBuffer->GetId().Id);
    //                 }
    //                 else if (binding.type == ResourceType::SamplerTexture) {
    //                     const auto* glTexture = static_cast<const GLTexture*>(binding.resourcePtr);
    //                     glBindTextureUnit(bindingPoint, renderDevice->GetTextureData(glTexture->GetId()).id);
    //                 }
    //             }
    //         }
    //
    //         case GLCommandType::Draw: {
    //             const auto* cmd = reinterpret_cast<GLCommand_Draw*>(&buffer[readOffset]);
    //             readOffset += sizeof(GLCommand_Draw);
    //
    //             glBindVertexArray(vao);
    //
    //             glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(cmd->vertexCount), GL_UNSIGNED_INT, nullptr);
    //
    //             glBindVertexArray(0);
    //             break;
    //         }
    //
    //
    //
    //         case GLCommandType::Clear: {
    //             glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //             break;
    //         }
    //
    //         default:
    //             break;
    //     }
    // }
    //
    //std::ranges::fill(buffer, 0.0f);

    head = 0;
}
