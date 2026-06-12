//
// Created by zykov on 6/1/2026.
//

#ifndef MAXENGINE_RENDERCONFIG_H
#define MAXENGINE_RENDERCONFIG_H

namespace Rendering {
    enum class RenderAPI {
        OpenGL,
        Vulkan,
        DX12
    };

    inline std::ostream& operator<<(std::ostream& os, const RenderAPI api) {
        switch (api) {
            case RenderAPI::OpenGL:   os << "OpenGL";   break;
            case RenderAPI::Vulkan: os << "Vulkan"; break;
            case RenderAPI::DX12:  os << "DX12";  break;
        }
        return os;
    }

    class RenderConfig {
    public:
        RenderAPI api = RenderAPI::OpenGL;
        Shader* litShader;
        Shader* shadowShader;
    };
} // Rendering

#endif //MAXENGINE_RENDERCONFIG_H
