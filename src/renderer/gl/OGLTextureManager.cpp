//
// Created by zykov on 6/26/2026.
//

#include "OGLTextureManager.h"
#include "model/TextureData.h"

namespace Rendering {
    OGLTextureManager::OGLTextureManager() {
    }

    OGLTextureManager::~OGLTextureManager() {
        std::vector<uint32_t> ids;
        ids.reserve(textures.size());

        std::ranges::transform(textures, std::back_inserter(ids), [](const auto& it) {
            return it.Id;
        });

        glDeleteTextures(textures.size(), ids.data());
    }

    TextureHandle OGLTextureManager::CreateTexture(const TextureCreateRequest &request, const void *data) {
        OGLTexture texture = CreateTextureInternal(request);

        textures.emplace_back(texture);

        const TextureHandle textureHandle = { .Id = static_cast<uint32_t>(textures.size() - 1) };

        if (data) {
            UploadData(textureHandle, request.Width, request.Height, request.Format, data);
        }

        return textureHandle;
    }

    TextureHandle OGLTextureManager::CreateTexture(const TextureCreateRequest &request, const std::vector<void *> &datas) {
        OGLTexture texture = CreateTextureInternal(request);

        textures.emplace_back(texture);

        const TextureHandle textureHandle = { .Id = static_cast<uint32_t>(textures.size() - 1) };

        if (datas.size() > 0) {
            UploadData(textureHandle, request.Width, request.Height, request.Format, datas);
        }

        return textureHandle;
    }

    OGLTexture OGLTextureManager::CreateTextureInternal(const TextureCreateRequest &request) {
        const GLenum internalFormat = MapTextureInternalFormat(request.Format);
        const GLenum _wrap = MapTextureWrap(request.Wrap);
        const GLenum magFilter = MapTextureMagFilter(request.Filter);
        GLenum minFilter;
        const bool _useMipMaps = request.useMipMaps && request.MipLevels > 1;

        if (_useMipMaps) {
            minFilter = MapTextureMipmapMinFilter(request.Filter);
        } else {
            minFilter = MapTextureMinFilter(request.Filter);
        }

        GLenum textureType;
        if (request.Type == TextureType::Cube) {
            textureType = GL_TEXTURE_CUBE_MAP;
        } else {
            textureType = GL_TEXTURE_2D;
        }

        GLuint texture;
        glCreateTextures(textureType, 1, &texture);

        glTextureStorage2D(texture, request.MipLevels, internalFormat, request.Width, request.Height);

        glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, minFilter);
        glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, magFilter);
        glTextureParameteri(texture, GL_TEXTURE_WRAP_S, _wrap);
        glTextureParameteri(texture, GL_TEXTURE_WRAP_T, _wrap);
        if (request.Type == TextureType::Cube) {
            glTextureParameteri(texture, GL_TEXTURE_WRAP_R, _wrap);
        }

        if (_useMipMaps) {
            glGenerateTextureMipmap(texture);
        }

        if (request.Type == TextureType::Cube) {

        }

        if (request.Format == TextureFormat::DEPTH) {
            constexpr float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
            glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            if (request.Type == TextureType::Cube) {
                glTextureParameteri(texture, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
            }
            glTextureParameterfv(texture, GL_TEXTURE_BORDER_COLOR, borderColor);
            glTextureParameteri(texture, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        }

        return OGLTexture{.Id = texture};
    }

    void OGLTextureManager::UploadData(const TextureHandle handle, const size_t width, const size_t height,
                                       const TextureFormat format,
                                       const void *data) {
        const GLenum externalFormat = MapTextureExternalFormat(format);
        const OGLTexture &texture = GetTexture(handle);
        glTextureSubImage2D(texture.Id, 0, 0, 0, width, height, externalFormat, GL_UNSIGNED_BYTE, data);
    }

    void OGLTextureManager::UploadData(const TextureHandle handle, const size_t width, const size_t height,
                                       const TextureFormat format,
                                       const std::vector<void *> &datas) {
        const GLenum externalFormat = MapTextureExternalFormat(format);
        const OGLTexture &texture = GetTexture(handle);

        for (int i = 0; i < datas.size(); ++i) {
            if (datas[i]) {
                glTextureSubImage3D(
                    texture.Id,
                    0,
                    0, 0, i,
                    width, height, 1,
                    externalFormat,
                    GL_UNSIGNED_BYTE,
                    datas[i]
                );
            }
        }
    }

    OGLTexture &OGLTextureManager::GetTexture(const TextureHandle texture) {
        return textures[texture.Id];
    }
} // Rendering
