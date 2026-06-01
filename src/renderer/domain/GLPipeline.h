//
// Created by zykov on 5/22/2026.
//

#ifndef MAXENGINE_GLPIPELINE_H
#define MAXENGINE_GLPIPELINE_H
#include "IPipeline.h"
#include "../../../cmake-build-debug/_deps/glfw-src/src/internal.h"

namespace Rendering {
    enum class CullMode : uint8_t {
        None,
        Front,
        Back
    };

    enum class BlendFactor : uint8_t {
        Zero,
        One,
        SrcAlpha,
        OneMinusSrcAlpha
    };

    struct RenderState {
        // Depth Test
        bool depthTestEnable       : 1 = true;
        bool depthWriteEnable      : 1 = true;

        // Face Culling
        CullMode cullMode              = CullMode::Back;
        bool frontFaceClockwise    : 1 = false;

        // Color Blending
        bool blendEnable           : 1 = false;
        BlendFactor srcColorFactor     = BlendFactor::SrcAlpha;
        BlendFactor dstColorFactor     = BlendFactor::OneMinusSrcAlpha;

        bool wireframe             : 1 = false;
    };

    enum class ShaderType {
        Vertex,
        Fragment
    };

    struct ShaderInput {
        const char* Source;
        ShaderType Type;
    };

    class GLPipeline : public IPipeline {
    public:
        ~GLPipeline() override;

        void Compile(const std::vector<ShaderInput> &shaders, RenderState renderState) override;

        void Bind(ICommandBuffer* commandBuffer) const override;

        void Unbind() const override;

        GLuint GetProgram() const;

        RenderState GetRenderState() const;

    private:
        GLuint program{};
        RenderState renderState{};

    };
} // Rendering

#endif //MAXENGINE_GLPIPELINE_H
