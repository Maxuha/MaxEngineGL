//
// Created by zykov on 6/26/2026.
//

#include "OGLTextureManager.h"
#include "model/TextureData.h"

namespace Rendering {
    OGLTextureManager::OGLTextureManager() {
    }

    OGLTextureManager::~OGLTextureManager() {
    }

    TextureHandle OGLTextureManager::CreateTexture(const size_t width, const size_t height, const TextureFormat format, void *data = nullptr) {
        GLuint texture;
        glCreateTextures(GL_TEXTURE_2D, 1, &texture);

        glTextureStorage2D(texture, 1, MapTextureInternalFormat(format), width, height);

        glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        textures.emplace_back(OGLTexture{texture});

        if (data) {
            glTextureSubImage2D(texture, 0, 0, 0, width, height, MapTextureExternalFormat(format), GL_UNSIGNED_BYTE, data);
        }

        return {textures.size() - 1};
    }

    OGLTexture OGLTextureManager::GetTexture(TextureHandle texture) const {
        return textures[texture.Id];
    }

    GLenum OGLTextureManager::MapTextureInternalFormat(const TextureFormat format) {
        switch (format) {
            case TextureFormat::SRGBA: return GL_SRGB8_ALPHA8;
            case TextureFormat::RGBA: return GL_RGBA8;
            case TextureFormat::RGB: return GL_RGB8;
            case TextureFormat::D32_FLOAT: return GL_DEPTH_COMPONENT32F;
            case TextureFormat::DEPTH: return GL_DEPTH_COMPONENT24;
        }
        return GL_RGBA8;
    }

    GLenum OGLTextureManager::MapTextureExternalFormat(const TextureFormat format) {
        switch (format) {
            case TextureFormat::SRGBA: return GL_RGBA;
            case TextureFormat::RGBA: return GL_RGBA;
            case TextureFormat::RGB: return GL_RGB;
            case TextureFormat::DEPTH: return GL_DEPTH_COMPONENT;
            case TextureFormat::D32_FLOAT: return GL_DEPTH_COMPONENT;
        }
        return GL_RGBA;
    }
} // Rendering