//
// Created by zykov on 6/19/2026.
//

#ifndef MAXENGINE_ISHADERMANAGER_H
#define MAXENGINE_ISHADERMANAGER_H

namespace Rendering {
    enum class ShaderType { Vertex, Fragment };

    struct ShaderHandle {
        uint32_t Id;
    };

    class ILinkPipeline {
        public:
        virtual ~ILinkPipeline() = default;
    };

    class IShaderManager {
        public:
        virtual ~IShaderManager() = default;

        virtual ShaderHandle CreateShader(ShaderType type, const std::vector<char>& code) = 0;

        virtual ShaderHandle CreateShader(ShaderType type, const char* code) = 0;
    };
} // Rendering

#endif //MAXENGINE_ISHADERMANAGER_H
