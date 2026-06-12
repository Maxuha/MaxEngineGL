//
// Created by zykov on 5/12/2026.
//

#include "GLTexture.h"
#include <iostream>
#include "../RendererContext.h"

namespace Rendering {
    GLTexture::GLTexture(const size_t width, const size_t height) {
        this->width = width;
        this->height = height;

        TextureCreateRequest desc;
        desc.Width = width;
        desc.Height = height;
        desc.Format = TextureFormat::DEPTH;
        desc.MipLevels = 1;

        id = RendererContext::GetContext()->CreateTexture(nullptr, desc);
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

        id = RendererContext::GetContext()->CreateTexture(data, desc);
    }

    TextureHandle GLTexture::GetId() {
        return id;
    }

    size_t GLTexture::GetWidth() const {
        return width;
    }

    size_t GLTexture::GetHeight() const {
        return height;
    }

    void GLTexture::SetWrap(const TextureWrap wrap) {
        RendererContext::GetContext()->UpdateTextureFilterMode(id, wrap);
    }

    TextureWrap GLTexture::GetWrap() {
        return TextureWrap::ClampToBorder;
    }
} // Rendering
