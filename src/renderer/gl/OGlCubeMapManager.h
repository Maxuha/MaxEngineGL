//
// Created by zykov on 10/20/2026.
//

#ifndef MAXENGINE_OGLCUBEMAPMANAGER_H
#define MAXENGINE_OGLCUBEMAPMANAGER_H
#include "../IRenderDevice.h"
#include "model/TextureData.h"

namespace Rendering {
    struct OGLCubeMapData {
        GLuint glId = 0;
    };

    class OGlCubeMapManager {
    public:
        explicit OGlCubeMapManager();
        ~OGlCubeMapManager();

        CubeMapHandle CreateCubeMap(const TextureCreateRequest &desc, const std::vector<void *> &datas);

        OGLCubeMapData& GetCubeMap(CubeMapHandle cubeMap);

    private:
        std::vector<OGLCubeMapData> cubeMaps;

        GLenum MapTextureInternalFormat(const TextureFormat format) {
            switch (format) {
                case TextureFormat::SRGBA: return GL_SRGB8_ALPHA8;
                case TextureFormat::RGBA: return GL_RGBA8;
                case TextureFormat::RGB: return GL_RGB8;
                case TextureFormat::DEPTH: return GL_DEPTH_COMPONENT24;
                case TextureFormat::UNDEFINED:
                    break;
            }
            return GL_RGBA8;
        }

        GLenum MapTextureExternalFormat(const TextureFormat format) {
            switch (format) {
                case TextureFormat::SRGBA: return GL_RGBA;
                case TextureFormat::RGBA: return GL_RGBA;
                case TextureFormat::RGB: return GL_RGB;
                case TextureFormat::DEPTH: return GL_DEPTH_COMPONENT;
            }
            return GL_RGBA;
        }
    };
} // Rendering

#endif //MAXENGINE_OGLCUBEMAPMANAGER_H
