//
// Created by zykov on 5/12/2026.
//

#include "Texture.h"
#include <iostream>
#include "../RenderContext.h"
#include "../../di/DIContainer.h"

namespace Rendering {
    Texture::Texture(const size_t width, const size_t height) {
        this->width = width;
        this->height = height;

        TextureCreateRequest desc;
        desc.Width = width;
        desc.Height = height;
        desc.Format = TextureFormat::DEPTH;
        desc.MipLevels = 1;

        // id = RenderContext::GetContext()->CreateTexture(nullptr, desc);
    }

    Texture::Texture(const size_t width, const size_t height, const TextureWrap wrap, void *data) {
        this->width = width;
        this->height = height;
        this->wrap = wrap;

        TextureCreateRequest desc;
        desc.Width = width;
        desc.Height = height;
        desc.Format = TextureFormat::SRGBA;
        desc.MipLevels = 1;

        TextureCreateRequest request;
        request.Width = width;
        request.Height = height;
        request.Format = TextureFormat::SRGBA;
        request.MipLevels = 1;
        request.ArraySize = width * height * 4;

         id = DIContainer::GetInstance().Get<IRenderer>()->CreateTexture(data, request);
    }

    TextureHandle Texture::GetId() const {
        return id;
    }

    size_t Texture::GetWidth() const {
        return width;
    }

    size_t Texture::GetHeight() const {
        return height;
    }

    void Texture::SetWrap(const TextureWrap wrap) {
        // RenderContext::GetContext()->UpdateTextureFilterMode(id, wrap);
    }

    TextureWrap Texture::GetWrap() {
        return TextureWrap::ClampToBorder;
    }
} // Rendering
