//
// Created by zykov on 6/26/2026.
//

#ifndef MAXENGINE_OGLPIPELINEMANAGER_H
#define MAXENGINE_OGLPIPELINEMANAGER_H
#include "../IPipelineManager.h"
#include "../domain/struct/VertexLayout.h"
#include "../gl/model/OGLPipelineLayout.h"

namespace Rendering {
    struct RenderContext;
    class OGLShaderManager;
    struct RenderPassHandle;
    struct PipelineStateDesc;
    struct ShaderHandle;
    struct PipelineHandle;
    struct OGLGraphicsShader;

    struct OGLPipeline {
        uint32_t vao{};
        GLuint program{};
        GLboolean depthTestEnable{};
        GLboolean depthWriteEnable{};
        GLboolean cullEnable{};
        GLenum cullMode{};
        GLenum fillMode{};
        OGLPipelineLayout layout{};
    };

    class OGLPipelineManager : public IPipelineManager {
    public:
        explicit OGLPipelineManager();

        ~OGLPipelineManager() override;

        PipelineHandle CreatePipeline(const std::vector<ShaderHandle> &shaders, RenderPassHandle renderPass,
                                      const PipelineStateDesc &pipelineStateDesc) override;

        PipelineHandle CreatePipeline(const OGLGraphicsShader& shader, const PipelineStateDesc &pipelineStateDesc);

        OGLPipeline GetPipeline(const PipelineHandle &pipelineHandle) const;

    private:
        std::vector<OGLPipeline> pipelines;

        GLuint MapVertexFormat(const VertexFormat format) {
            switch (format) {
                case VertexFormat::Float:
                    return 1;
                    break;
                case VertexFormat::Float2:
                    return 2;
                    break;
                case VertexFormat::Float3:
                    return 3;
                    break;
                case VertexFormat::Float4:
                    return 4;
                    break;
                case VertexFormat::UByte4N:
                    return 4;
                    break;
                case VertexFormat::Int4:
                    return 4;
                    break;
            }
            return 1;
        }
    };
} // Rendering

#endif //MAXENGINE_OGLPIPELINEMANAGER_H
