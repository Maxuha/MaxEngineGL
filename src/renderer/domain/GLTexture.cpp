//
// Created by zykov on 5/12/2026.
//

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "GLTexture.h"
#include <iostream>
#include "../../di/DIContainer.h"

namespace Rendering {
    GLTexture::GLTexture(const size_t width, const size_t height) {
        this->width = width;
        this->height = height;

        TextureCreateRequest desc;
        desc.Width = width;
        desc.Height = height;
        desc.Format = TextureFormat::DEPTH;
        desc.MipLevels = 1;

        id = DIContainer::GetInstance().Get<IRenderer>()->CreateTexture(nullptr, desc);
    }

    GLTexture::GLTexture(const size_t width, const size_t height, const TextureWrap wrap, void *data) {
        this->width = width;
        this->height = height;
        this->wrap = wrap;

        TextureCreateRequest desc;
        desc.Width = width;
        desc.Height = height;
        desc.Format = TextureFormat::RGBA;
        desc.MipLevels = 1;

        id = DIContainer::GetInstance().Get<IRenderer>()->CreateTexture(data, desc);
    }

    TextureId GLTexture::GetId() {
        return id;
    }

    size_t GLTexture::GetWidth() const {
        return width;
    }

    size_t GLTexture::GetHeight() const {
        return height;
    }

    void GLTexture::SetWrap(const TextureWrap wrap) {
        DIContainer::GetInstance().Get<IRenderer>()->UpdateTextureFilterMode(id, wrap);
    }

    TextureWrap GLTexture::GetWrap() {
        return TextureWrap::ClampToBorder;
    }
} // Rendering
