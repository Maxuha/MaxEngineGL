//
// Created by zykov on 6/26/2026.
//

#include "OGLPipelineManager.h"

#include <iostream>

#include "OGLRenderDevice.h"
#include "OGLShaderManager.h"
#include "../IRenderPassManager.h"
#include "../../math/Vertex.h"
#include "../domain/struct/PipelineStateDesc.h"
#include "../managers/IShaderManager.h"


namespace Rendering {
    OGLPipelineManager::OGLPipelineManager() {
    }

    OGLPipelineManager::~OGLPipelineManager() {
    }

    PipelineHandle OGLPipelineManager::CreatePipeline(const std::vector<ShaderHandle> &shaders,
        const RenderPassHandle renderPass, const PipelineStateDesc &pipelineStateDesc) {

        const GLuint program = glCreateProgram();

        int success;
        GLchar infoLog[512];

        glGetProgramiv(program, GL_LINK_STATUS, &success);

        if (!success) {
            glGetProgramInfoLog(program, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        for (const auto &binding : pipelineStateDesc.VertexLayout.bindings) {
            const GLuint divisor = (binding.stepRate == VertexStepRate::PerInstance) ? 1 : 0;
            glVertexArrayBindingDivisor(vao, binding.bufferIndex, divisor);
        }

        for (const auto &attribute : pipelineStateDesc.VertexLayout.attributes) {
            glEnableVertexArrayAttrib(vao, attribute.location);
            glVertexArrayAttribFormat(vao, attribute.location, MapVertexFormat(attribute.format), GL_FLOAT, GL_FALSE, attribute.offset);
            glVertexArrayAttribBinding(vao, attribute.location, attribute.bufferIndex);
        }

        glBindVertexArray(0);

        const PipelineHandle pipelineHandle = { .Id = pipelines.size() };

        OGLPipeline pipeline;
        pipeline.cullEnable = true;
        pipeline.program = program;
        pipeline.cullMode = GL_FRONT;
        pipeline.fillMode = GL_FILL;
        pipeline.depthTestEnable = true;
        pipeline.depthWriteEnable = true;
        pipeline.vao = vao;

        pipelines.push_back(pipeline);

        return pipelineHandle;
    }

    PipelineHandle OGLPipelineManager::CreatePipeline(const OGLGraphicsShader& shader, const PipelineStateDesc &pipelineStateDesc) {

        const GLuint program = glCreateProgram();

        glAttachShader(program, shader.vShader);
        glAttachShader(program, shader.fShader);

        glLinkProgram(program);

        glDetachShader(program, shader.vShader);
        glDetachShader(program, shader.fShader);

        int success;
        GLchar infoLog[512];

        glGetProgramiv(program, GL_LINK_STATUS, &success);

        if (!success) {
            glGetProgramInfoLog(program, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        for (const auto &binding : pipelineStateDesc.VertexLayout.bindings) {
            const GLuint divisor = (binding.stepRate == VertexStepRate::PerInstance) ? 1 : 0;
            glVertexArrayBindingDivisor(vao, binding.bufferIndex, divisor);
        }

        for (const auto &attribute : pipelineStateDesc.VertexLayout.attributes) {
            glEnableVertexArrayAttrib(vao, attribute.location);
            glVertexArrayAttribFormat(vao, attribute.location, MapVertexFormat(attribute.format), GL_FLOAT, GL_FALSE, attribute.offset);
            glVertexArrayAttribBinding(vao, attribute.location, attribute.bufferIndex);
        }

        glBindVertexArray(0);

        const PipelineHandle pipelineHandle = { .Id = pipelines.size() };

        OGLPipelineLayout layout;
        layout.resources = shader.layouts;

        OGLPipeline pipeline;
        pipeline.cullEnable = true;
        pipeline.program = program;
        pipeline.cullMode = GL_FRONT;
        pipeline.fillMode = GL_FILL;
        pipeline.depthTestEnable = true;
        pipeline.depthWriteEnable = true;
        pipeline.vao = vao;
        pipeline.layout = layout;

        pipelines.push_back(pipeline);

        return pipelineHandle;
    }

    OGLPipeline OGLPipelineManager::GetPipeline(const PipelineHandle &pipelineHandle) const {
        return pipelines[pipelineHandle.Id];
    }
} // Rendering