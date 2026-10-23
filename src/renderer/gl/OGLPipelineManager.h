//
// Created by zykov on 6/26/2026.
//

#ifndef MAXENGINE_OGLPIPELINEMANAGER_H
#define MAXENGINE_OGLPIPELINEMANAGER_H
#include "../IPipelineManager.h"
#include "../domain/struct/VertexLayout.h"
#include "../gl/model/OGLPipelineLayout.h"
#include "../domain/struct/PipelineStateDesc.h"

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
        GLenum depthCompareFunc{};
        GLboolean cullEnable{};
        GLenum cullMode{};
        GLenum fillMode{};
        OGLPipelineLayout layout{};
    };

    class OGLPipelineManager : public IPipelineManager {
    public:
        explicit OGLPipelineManager();

        ~OGLPipelineManager() override;


        PipelineHandle CreatePipeline(const OGLGraphicsShader& shader, const PipelineStateDesc &pipelineStateDesc);

        OGLPipeline GetPipeline(const PipelineHandle &pipelineHandle) const;

    private:
        std::vector<OGLPipeline> pipelines;

        GLenum MapRenderFace(RenderFace face);

        GLuint MapVertexFormat(const VertexFormat format) {
            switch (format) {
                case VertexFormat::Float:
                    return 1;
                case VertexFormat::Float2:
                    return 2;
                case VertexFormat::Float3:
                    return 3;
                case VertexFormat::Float4:
                    return 4;
                case VertexFormat::UByte4N:
                    return 4;
                case VertexFormat::Int4:
                    return 4;
            }
            return 1;
        }

        GLenum MapDepthCompareOp(const DepthCompareOp op) {
            switch (op) {
                case DepthCompareOp::Never:          return GL_NEVER;
                case DepthCompareOp::Less:           return GL_LESS;
                case DepthCompareOp::Equal:          return GL_EQUAL;
                case DepthCompareOp::LessOrEqual:    return GL_LEQUAL;
                case DepthCompareOp::Greater:        return GL_GREATER;
                case DepthCompareOp::NotEqual:       return GL_NOTEQUAL;
                case DepthCompareOp::GreaterOrEqual: return GL_GEQUAL;
                case DepthCompareOp::Always:         return GL_ALWAYS;
            }
            return GL_LEQUAL;
        }
    };
} // Rendering

#endif //MAXENGINE_OGLPIPELINEMANAGER_H
