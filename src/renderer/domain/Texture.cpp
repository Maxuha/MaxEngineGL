//
// Created by zykov on 5/12/2026.
//

#include "Texture.h"
#include "../../di/DIContainer.h"

namespace Rendering {
    Texture::Texture(const size_t width, const size_t height, void *data) {
        this->width = width;
        this->height = height;

        TextureCreateRequest desc;
        desc.Width = width;
        desc.Height = height;
        desc.Format = TextureFormat::SRGBA;
        desc.Filter = TextureFilter::Point;
        desc.Wrap = TextureWrap::Clamp;

        id = DIContainer::GetInstance().Get<IRenderer>()->CreateTexture(desc, data);
    }

    Texture::Texture(const size_t width, const size_t height, const std::vector<void *> &data) {
        this->width = width;
        this->height = height;

        TextureCreateRequest desc;
        desc.Width = width;
        desc.Height = height;
        desc.Type = TextureType::Cube;

        id = DIContainer::GetInstance().Get<IRenderer>()->CreateTexture(desc, data);
    }

    Texture::Texture(const size_t width, const size_t height, const TextureType type) {
        this->width = width;
        this->height = height;

        TextureCreateRequest desc;
        desc.Width = width;
        desc.Height = height;
        desc.Type = type;

        id = DIContainer::GetInstance().Get<IRenderer>()->CreateTexture(desc, nullptr);
    }

    Texture::Texture(const size_t width, const size_t height, const TextureType type, const uint32_t mipLevels) {
        this->width = width;
        this->height = height;

        TextureCreateRequest desc;
        desc.Width = width;
        desc.Height = height;
        desc.Type = type;
        desc.MipLevels = mipLevels;
        desc.useMipMaps = mipLevels > 1;

        id = DIContainer::GetInstance().Get<IRenderer>()->CreateTexture(desc, nullptr);
    }

    Texture::Texture(const size_t width, const size_t height, void* data, const TextureFormat format) {
        this->width = width;
        this->height = height;

        TextureCreateRequest desc;
        desc.Width = width;
        desc.Height = height;
        desc.Type = TextureType::Texture2D;
        desc.Format = format;

        id = DIContainer::GetInstance().Get<IRenderer>()->CreateTexture(desc, data);
    }

    Texture::Texture(const size_t width, const size_t height, const TextureWrap wrap, const TextureFilter filter, const TextureFormat format,
                     const uint32_t mipLevels, void *data) {
        this->width = width;
        this->height = height;
        this->wrap = wrap;

        TextureCreateRequest request;
        request.Width = width;
        request.Height = height;
        request.Format = format;
        request.Wrap = wrap;
        request.Filter = filter;
        request.MipLevels = mipLevels;
        request.useMipMaps = mipLevels > 1;

        id = DIContainer::GetInstance().Get<IRenderer>()->CreateTexture(request, data);
    }

    Texture::Texture(const size_t width, const size_t height, const TextureWrap wrap, const TextureFilter filter, const TextureFormat format,
        const uint32_t mipLevels, const std::vector<void *> &data) {

        this->width = width;
        this->height = height;
        this->wrap = wrap;

        TextureCreateRequest request;
        request.Width = width;
        request.Height = height;
        request.Format = format;
        request.Wrap = wrap;
        request.Filter = filter;
        request.MipLevels = mipLevels;
        request.useMipMaps = mipLevels > 1;

        id = DIContainer::GetInstance().Get<IRenderer>()->CreateTexture(request, data);
    }

    Texture::Texture(const size_t width, const size_t height, const TextureWrap wrap, const TextureFilter filter, const TextureFormat format,
        const uint32_t mipLevels, const TextureType type) {
        this->width = width;
        this->height = height;
        this->wrap = wrap;

        TextureCreateRequest request;
        request.Width = width;
        request.Height = height;
        request.Format = format;
        request.Wrap = wrap;
        request.Filter = filter;
        request.MipLevels = mipLevels;
        request.useMipMaps = mipLevels > 1;
        request.Type = type;

        id = DIContainer::GetInstance().Get<IRenderer>()->CreateTexture(request, nullptr);
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
        return TextureWrap::Clamp;
    }
} // Rendering
