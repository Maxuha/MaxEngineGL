//
// Created by zykov on 5/22/2026.
//

#include "GLPipeline.h"

#include "ICommandBuffer.h"
#include "glad/glad.h"

namespace Rendering {

    GLPipeline::~GLPipeline() = default;

    void GLPipeline::Compile(const std::vector<ShaderInput> &shaders, RenderState renderState) {
        this->renderState = renderState;

        GLuint shaderIds[shaders.size()];

        program = glCreateProgram();

        for (int i = 0; i < shaders.size(); i++) {
            const ShaderType type = shaders[i].Type;
            const char* source = shaders[i].Source;

            GLuint shaderId = 0;
            switch (type) {
                case ShaderType::Vertex:
                    shaderId = glCreateShader(GL_VERTEX_SHADER);
                    break;
                case ShaderType::Fragment:
                    shaderId = glCreateShader(GL_FRAGMENT_SHADER);
                    break;
            }

            if (shaderId == 0) continue;

            glShaderSource(shaderId, 1, &source, nullptr);
            glCompileShader(shaderId);
            glAttachShader(program, shaderId);
            shaderIds[i] = shaderId;
        }

        glLinkProgram(program);

        for (const GLuint shaderId : shaderIds) {
            glDetachShader(program, shaderId);
            glDeleteShader(shaderId);
        }
    }

    void GLPipeline::Bind(ICommandBuffer* commandBuffer) const {
        //commandBuffer->BindPipeline(*this);
    }

    void GLPipeline::Unbind() const {
        glUseProgram(0);
    }

    GLuint GLPipeline::GetProgram() const {
        return program;
    }

    RenderState GLPipeline::GetRenderState() const {
        return renderState;
    }
} // Rendering