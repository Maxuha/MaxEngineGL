//
// Created by zykov on 6/26/2026.
//

#ifndef MAXENGINE_OGLTEXTUREMANAGER_H
#define MAXENGINE_OGLTEXTUREMANAGER_H

#include "../../renderer/ITextureManager.h"


namespace Rendering {
    struct OGLTexture {
        uint32_t Id = 0;
        uint32_t fbo = 0;
    };

    class OGLTextureManager {
    public:
        explicit OGLTextureManager();

        ~OGLTextureManager();

        TextureHandle CreateTexture(const TextureCreateRequest &request, const void* data = nullptr);

        TextureHandle CreateTexture(const TextureCreateRequest &request, const std::vector<void *> &datas);

        OGLTexture& GetTexture(TextureHandle texture);

    private:
        std::vector<OGLTexture> textures;

        OGLTexture CreateTextureInternal(const TextureCreateRequest &request);

        void UploadData(TextureHandle handle, size_t width, size_t height, TextureFormat format, const void *data);

        void UploadData(TextureHandle handle, size_t width, size_t height, TextureFormat format, const std::vector<void *> &datas);

         GLenum MapTextureInternalFormat(const TextureFormat format) {
            switch (format) {
                case TextureFormat::SRGBA: return GL_SRGB8_ALPHA8;
                case TextureFormat::RGBA: return GL_RGBA8;
                case TextureFormat::RGB: return GL_RGB8;
                case TextureFormat::DEPTH: return GL_DEPTH_COMPONENT24;
                case TextureFormat::RG16F:
                    return GL_RG16F;
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
                case TextureFormat::RG16F:
                    return GL_RG;
                case TextureFormat::UNDEFINED:
                    break;
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
                case TextureFilter::Point:     return GL_NEAREST;
                case TextureFilter::Bilinear:
                case TextureFilter::Trilinear: return GL_LINEAR;
                default:                       return GL_LINEAR;
            }
        }

        static constexpr GLenum MapTextureMipmapMinFilter(const TextureFilter filter) {
            switch (filter) {
                case TextureFilter::Point:     return GL_NEAREST_MIPMAP_NEAREST;
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

#endif //MAXENGINE_OGLTEXTUREMANAGER_H
