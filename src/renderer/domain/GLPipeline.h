//
// Created by zykov on 5/22/2026.
//

#ifndef MAXENGINE_GLPIPELINE_H
#define MAXENGINE_GLPIPELINE_H
#include <utility>

#include "IPipeline.h"
#include "../../../cmake-build-debug/_deps/glfw-src/src/internal.h"
#include "struct/PipelineStateDesc.h"

namespace Rendering {

    enum class ShaderType {
        Vertex,
        Fragment
    };

    struct ShaderInput {
        std::string Source;
        ShaderType Type;
    };

    class GLPipeline : public IPipeline {
    public:
        explicit GLPipeline(PipelineStateDesc pipelineStateDesc)
            : pipelineStateDesc(std::move(pipelineStateDesc)) {
        }

        ~GLPipeline() override;

        void Compile(const std::vector<ShaderInput> &shaders) override;

        void Bind(ICommandBuffer* commandBuffer) const override;

        void Unbind() const override;

        GLuint GetProgram() const;

        PipelineStateDesc GetRenderState() const;

    private:
        GLuint program{};
        PipelineStateDesc pipelineStateDesc{};

    };
} // Rendering

#endif //MAXENGINE_GLPIPELINE_H
