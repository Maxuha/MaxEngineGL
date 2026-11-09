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
        GLuint fbo = 0;
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

        static constexpr GLenum MapTextureWrap(const TextureWrap wrap) {
            switch (wrap) {
                case TextureWrap::Repeat:     return GL_REPEAT;
                case TextureWrap::Clamp:      return GL_CLAMP_TO_EDGE;
                case TextureWrap::Mirror:     return GL_MIRRORED_REPEAT;
                case TextureWrap::MirrorOnce: return GL_MIRROR_CLAMP_TO_EDGE;
                default:                      return GL_REPEAT;
            }
        }

        static constexpr GLenum MapTextureMinFilter(const TextureFilter filter) {
            switch (filter) {
                case TextureFilter::Point:     return GL_NEAREST_MIPMAP_NEAREST;
                case TextureFilter::Bilinear:
                case TextureFilter::Trilinear: return GL_LINEAR;
                default:                       return GL_LINEAR;
            }
        }

        static constexpr GLenum MapTextureMipmapMinFilter(const TextureFilter filter) {
            switch (filter) {
                case TextureFilter::Point:     return GL_NEAREST;
                case TextureFilter::Bilinear:  return GL_LINEAR_MIPMAP_NEAREST;
                case TextureFilter::Trilinear: return GL_LINEAR_MIPMAP_LINEAR;
                default:                       return GL_LINEAR_MIPMAP_LINEAR;
            }
        }

        static constexpr int MapTextureMagFilter(const TextureFilter filter) {
            switch (filter) {
                case TextureFilter::Point:     return GL_NEAREST;
                case TextureFilter::Bilinear:
                case TextureFilter::Trilinear: return GL_LINEAR;
                default:                       return GL_LINEAR;
            }
        }
    };
} // Rendering

#endif //MAXENGINE_OGLCUBEMAPMANAGER_H
