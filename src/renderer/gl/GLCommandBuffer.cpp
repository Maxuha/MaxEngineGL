//
// Created by zykov on 5/22/2026.
//

#include "GLCommandBuffer.h"
#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"
#include "../domain/GLTexture.h"
#include "../../math/Color.h"

Rendering::GLCommandBuffer::GLCommandBuffer(const size_t bufferSize) {
    buffer.resize(bufferSize);
}

Rendering::GLCommandBuffer::~GLCommandBuffer() = default;

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

void Rendering::GLCommandBuffer::BeginRenderPass(const AttachmentDescription &attachment, const GLuint fboId,
    const ::Color clearColor) {
    auto *cmd = allocateCommand<GLCommand_BeginRenderPass>(GLCommandType::BeginRenderPass);
    cmd->attachment = attachment;
    cmd->fboId = fboId;
    cmd->clearColor = clearColor;
}

void Rendering::GLCommandBuffer::EndRenderPass(const AttachmentDescription& attachment) {
    auto *cmd = allocateCommand<GLCommand_EndRenderPass>(GLCommandType::EndRenderPass);
    cmd->attachment = attachment;
}

void Rendering::GLCommandBuffer::BindPipeline(const PipelineHandle &pipeline) {
    auto *cmd = allocateCommand<GLCommand_BindPipeline>(GLCommandType::BindPipeline);
    cmd->pipeline = pipeline;
    this->pipeline = pipeline;
}

void Rendering::GLCommandBuffer::BindVertexBuffer(BufferHandle &buffer) {
    auto *cmd = allocateCommand<GLCommand_BindBuffer>(GLCommandType::BindVertexBuffer);
    cmd->buffer = buffer;
}

void Rendering::GLCommandBuffer::BindIndexBuffer(BufferHandle &buffer) {
    auto *cmd = allocateCommand<GLCommand_BindBuffer>(GLCommandType::BindIndexBuffer);
    cmd->buffer = buffer;
}

void Rendering::GLCommandBuffer::BindUniformBuffer(const uint32_t bindingPoint, const BufferHandle &buffer) {
    auto *cmd = allocateCommand<GLCommand_BindUniform>(GLCommandType::BindUniformBuffer);
    cmd->bindingPoint = bindingPoint;
    cmd->buffer = buffer;
}

void Rendering::GLCommandBuffer::UpdateBuffer(const BufferHandle id, void *data, const int offset, const int size) {
    auto *cmd = allocateCommand<GLCommand_UpdateBuffer>(GLCommandType::UpdateBuffer);
    cmd->buffer = id;
    cmd->offset = offset;
    cmd->size = size;
    cmd->data = data;
}

void Rendering::GLCommandBuffer::BindResourceSet(uint32_t setIndex, const IResourceSet &resourceSet) {
    auto *cmd = allocateCommand<GLCommand_BindResourceSet>(GLCommandType::BindResourceSet);
    cmd->resourceSet = &dynamic_cast< const GLResourceSet& >(resourceSet);
}

void Rendering::GLCommandBuffer::PushConstants(const uint32_t key, const Matrix4x4 mat) {
    auto *cmd = allocateCommand<GLCommand_PushConstants>(GLCommandType::PushConstants);
    cmd->key = key;
    cmd->mat = mat;
}

void Rendering::GLCommandBuffer::PushConstants(const std::string &key, const uint32_t value) {
    auto *cmd = allocateCommand<GLCommand_PushConstants>(GLCommandType::PushConstants);
    cmd->value = value;
}

std::vector<uint8_t>& Rendering::GLCommandBuffer::GetCommands() {
    return buffer;
}

size_t Rendering::GLCommandBuffer::GetHead() const {
    return head;
}

Rendering::PipelineHandle Rendering::GLCommandBuffer::GetPipeline() const {
    return pipeline;
}

void Rendering::GLCommandBuffer::ResetAll() {
    head = 0;
}
