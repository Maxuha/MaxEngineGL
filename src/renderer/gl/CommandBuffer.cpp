//
// Created by zykov on 5/22/2026.
//

#include "CommandBuffer.h"
#include "glad/glad.h"
#include "../../math/Color.h"


Rendering::CommandBuffer::CommandBuffer(const size_t bufferSize) {
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

void Rendering::CommandBuffer::DrawIndexed(const uint32_t indexCount) {
    auto *cmd = allocateCommand<GLCommand_DrawIndexed>(GLCommandType::DrawIndexed);
    cmd->indexCount = indexCount;
}

void Rendering::CommandBuffer::BeginRenderPass(const AttachmentDescription &attachment,
                                               GLuint fboId, int w, int h,
                                               Color clearColor) {
    auto *cmd = allocateCommand<GLCommand_BeginRenderPass>(GLCommandType::BeginRenderPass);
    cmd->attachment1 = attachment;
    cmd->fboId = fboId;
    cmd->width = w;
    cmd->height = h;
    cmd->clearColor = clearColor;
}

void Rendering::CommandBuffer::BeginRenderPass(const AttachmentDescription &attachment, const GLuint fboId,
    const ::Color clearColor) {
    auto *cmd = allocateCommand<GLCommand_BeginRenderPass>(GLCommandType::BeginRenderPass);
    cmd->attachment1 = attachment;
    cmd->fboId = fboId;
    cmd->clearColor = clearColor;
}

void Rendering::CommandBuffer::BeginRenderPass(const RenderPassHandle renderPass, const FrameBufferHandle frameBuffer,
    const Rect &viewport, const Color clearColor) {
    auto *cmd = allocateCommand<GLCommand_BeginRenderPass>(GLCommandType::BeginRenderPass);
    cmd->renderPass = renderPass;
    cmd->frameBuffer = frameBuffer;
    cmd->viewport = viewport;
    cmd->clearColor = clearColor;

    AttachmentDescription attachment1{};
    attachment1.format = ImageFormat::RGBA8_Srgb;
    attachment1.loadOp = AttachmentLoadOp::CLEAR;
    attachment1.storeOp = AttachmentStoreOp::STORE;
    attachment1.type = AttachmentType::COLOR;

    AttachmentDescription attachment2{};
    attachment2.format = ImageFormat::D32_Sfloat_S8_Uint;
    attachment2.loadOp = AttachmentLoadOp::CLEAR;
    attachment2.storeOp = AttachmentStoreOp::DONT_CARE;
    attachment2.type = AttachmentType::DEPTH;

    cmd->attachment1 = attachment1;
    cmd->attachment2 = attachment2;
    cmd->width = viewport.width;
    cmd->height = viewport.height;
    cmd->clearColor = clearColor;
    cmd->fboId = -1;
}

void Rendering::CommandBuffer::BeginRenderPass(const RenderPassHandle renderPass, const Color clearColor) {
    auto *cmd = allocateCommand<GLCommand_BeginRenderPass>(GLCommandType::BeginRenderPass);
    cmd->renderPass = renderPass;
    cmd->clearColor = clearColor;
    cmd->frameBuffer = FrameBufferHandle(-1);
}

void Rendering::CommandBuffer::BeginRenderPass(const RenderPass &renderPass) {
    auto *cmd = allocateCommand<GLCommand_BeginRenderPass>(GLCommandType::BeginRenderPass);
    cmd->attachment1 = renderPass.colorAttachments[0];
    cmd->attachment2 = renderPass.colorAttachments[1];
    cmd->width = renderPass.viewport.width;
    cmd->height = renderPass.viewport.height;
    cmd->clearColor = renderPass.clearColor;
}

Rendering::CommandBufferHandle Rendering::CommandBuffer::GetHandle() const {
    return {};
}

Rendering::CommandPoolHandle Rendering::CommandBuffer::GetPool() const {
    return {};
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

void Rendering::CommandBuffer::BindVertexBuffer(BufferHandle &buffer) {
    auto *cmd = allocateCommand<GLCommand_BindBuffer>(GLCommandType::BindVertexBuffer);
    cmd->buffer = buffer;
}

void Rendering::CommandBuffer::BindIndexBuffer(BufferHandle &buffer) {
    auto *cmd = allocateCommand<GLCommand_BindBuffer>(GLCommandType::BindIndexBuffer);
    cmd->buffer = buffer;
}

void Rendering::CommandBuffer::UpdateBuffer(const BufferHandle id, const void *data, const int offset, const int size) {
    auto *cmd = allocateCommand<GLCommand_UpdateBuffer>(GLCommandType::UpdateBuffer);
    cmd->buffer = id;
    cmd->offset = offset;
    cmd->size = size;
    cmd->data = data;
}

void Rendering::CommandBuffer::BindResourceSet(const ResourceSetHandle setIndex, const PipelineHandle pipelineHandle, const uint32_t index) {
    auto *cmd = allocateCommand<GLCommand_BindResourceSet>(GLCommandType::BindResourceSet);
    cmd->resourceId = setIndex;
    cmd->pipeline = pipelineHandle;
    cmd->index = index;
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

void Rendering::CommandBuffer::UpdateUniformBuffer(const BufferHandle buffer, const void *data, const int offset, const int size) {
    auto *cmd = allocateCommand<GLCommand_UpdateUniformBuffer>(GLCommandType::UpdateUniformBuffer);
    cmd->buffer = buffer;
    cmd->offset = offset;
    cmd->size = size;
    cmd->data = data;
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

void Rendering::CommandBuffer::Reset() {
    head = 0;
}

void Rendering::CommandBuffer::ResetAll() {
    head = 0;
}
