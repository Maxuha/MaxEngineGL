//
// Created by zykov on 6/26/2026.
//

#ifndef MAXENGINE_OGLSHADERMANAGER_H
#define MAXENGINE_OGLSHADERMANAGER_H
#include "../managers/IShaderManager.h"

namespace Rendering {
    struct OGLShader {
        GLuint handle;
    };

    struct OGLLinkPipeline : ILinkPipeline {
        GLuint program;
    };

    class OGLShaderManager : public IShaderManager {
        public:
        explicit OGLShaderManager();
        ~OGLShaderManager() override;

        ShaderHandle CreateShader(ShaderType type, const std::vector<char> &code) override;

        ShaderHandle CreateShader(ShaderType type, const char *code) override;

        OGLShader GetShader(ShaderHandle handle) const;

    private:
        std::vector<OGLShader> shaders;

    };
} // Rendering

#endif //MAXENGINE_OGLSHADERMANAGER_H
