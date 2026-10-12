//
// Created by zykov on 5/22/2026.
//

#include "CommandBuffer.h"


Rendering::CommandBuffer::CommandBuffer(const size_t bufferSize, const CommandBufferType type) : type(type) {
    buffer.resize(bufferSize);
}

Rendering::CommandBuffer::~CommandBuffer() = default;

void Rendering::CommandBuffer::Begin() {
    head = 0;
}

void Rendering::CommandBuffer::End() {
}

void Rendering::CommandBuffer::Draw(const uint32_t vertexCount) {
    auto *cmd = allocateCommand<GLCommand_Draw>(GLCommandType::Draw);
    cmd->vertexCount = vertexCount;
}

void Rendering::CommandBuffer::DrawIndexed(const uint32_t indexCount, const uint32_t indexMesh) {
    auto *cmd = allocateCommand<GLCommand_DrawIndexed>(GLCommandType::DrawIndexed);
    cmd->indexCount = indexCount;
    cmd->indexMesh = indexMesh;
}

void Rendering::CommandBuffer::BeginRenderPass(const RenderPass &renderPass) {
    auto *cmd = allocateCommand<GLCommand_BeginRenderPass>(GLCommandType::BeginRenderPass);
    cmd->colorAttachment = renderPass.colorAttachment;
    cmd->depthAttachment = renderPass.depthAttachment;
    cmd->viewport = renderPass.viewport;
}

void Rendering::CommandBuffer::EndRenderPass() {
    auto *cmd = allocateCommand<GLCommand_EndRenderPass>(GLCommandType::EndRenderPass);
}

void Rendering::CommandBuffer::EndRenderPass(const AttachmentDescription& attachment) {
    auto *cmd = allocateCommand<GLCommand_EndRenderPass>(GLCommandType::EndRenderPass);
    cmd->attachment = attachment;
}

void Rendering::CommandBuffer::BindPipeline(const PipelineHandle &pipeline) {
    auto *cmd = allocateCommand<GLCommand_BindPipeline>(GLCommandType::BindPipeline);
    cmd->pipeline = pipeline;
    this->pipeline = pipeline;
}

void Rendering::CommandBuffer::BindMaterial(const MaterialHandle &material) {
    auto *cmd = allocateCommand<GLCommand_BindMaterial>(GLCommandType::BindMaterial);
    cmd->material = material;
}

void Rendering::CommandBuffer::BindVertexBuffer(const BufferHandle &buffer) {
    auto *cmd = allocateCommand<GLCommand_BindBuffer>(GLCommandType::BindVertexBuffer);
    cmd->buffer = buffer;
}

void Rendering::CommandBuffer::BindIndexBuffer(const BufferHandle &buffer) {
    auto *cmd = allocateCommand<GLCommand_BindBuffer>(GLCommandType::BindIndexBuffer);
    cmd->buffer = buffer;
}

void Rendering::CommandBuffer::BindResourceSet(const ResourceSetHandle setIndex, const PipelineHandle pipelineHandle, const uint32_t index, const uint32_t offset) {
    auto *cmd = allocateCommand<GLCommand_BindResourceSet>(GLCommandType::BindResourceSet);
    cmd->resourceId = setIndex;
    cmd->pipeline = pipelineHandle;
    cmd->isPipelineLayout = false;
    cmd->index = index;
    cmd->dynamicOffset = offset;
}

void Rendering::CommandBuffer::BindResourceSet(const ResourceSetHandle setIndex, const PipelineLayoutHandle pipelineLayoutHandle,
    const uint32_t index, const uint32_t offset) {
    auto *cmd = allocateCommand<GLCommand_BindResourceSet>(GLCommandType::BindResourceSet);
    cmd->resourceId = setIndex;
    cmd->pipelineLayout = pipelineLayoutHandle;
    cmd->isPipelineLayout = true;
    cmd->index = index;
    cmd->dynamicOffset = offset;
}

void Rendering::CommandBuffer::PushConstants(const uint32_t key, const Matrix4x4 mat) {
    auto *cmd = allocateCommand<GLCommand_PushConstants>(GLCommandType::PushConstants);
    cmd->key = key;
    cmd->mat = mat;
}

void Rendering::CommandBuffer::PushConstants(const std::string &key, const uint32_t value) {
    auto *cmd = allocateCommand<GLCommand_PushConstants>(GLCommandType::PushConstants);
    cmd->value = value;
}

void Rendering::CommandBuffer::UpdateBuffer(const BufferHandle buffer, const void *data, const int offset, const int size) {
    auto *cmd = allocateCommand<GLCommand_UpdateBuffer>(GLCommandType::UpdateBuffer);
    cmd->buffer = buffer;
    cmd->offset = offset;
    cmd->size = size;
    cmd->data = data;
}

void Rendering::CommandBuffer::UploadBufferData(const BufferHandle buffer, const void *data, const int offset, const int size) {
    auto *cmd = allocateCommand<GLCommand_UploadBufferData>(GLCommandType::UploadBufferData);

    cmd->buffer = buffer;
    cmd->offset = offset;
    cmd->size = size;
    cmd->data = data;

    std::memcpy(&this->buffer[this->head], data, size);

    head += size;
}

std::vector<uint8_t>& Rendering::CommandBuffer::GetCommands() {
    return buffer;
}

size_t Rendering::CommandBuffer::GetHead() const {
    return head;
}

Rendering::PipelineHandle Rendering::CommandBuffer::GetPipeline() const {
    return pipeline;
}

Rendering::CommandBufferType Rendering::CommandBuffer::GetType() const {
    return type;
}

void Rendering::CommandBuffer::Reset() {
    head = 0;
}
