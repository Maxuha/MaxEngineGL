//
// Created by zykov on 5/12/2026.
//

#ifndef MAXENGINE_GLTEXTURE_H
#define MAXENGINE_GLTEXTURE_H

namespace Rendering {
    class Texture {
    public:
        Texture(size_t width, size_t height, void* data);

        Texture(size_t width, size_t height, const std::vector<void*>& data);

        Texture(size_t width, size_t height, TextureType type);

        Texture(size_t width, size_t height, TextureType type, uint32_t mipLevels);

        Texture(size_t width, size_t height, void* data, TextureFormat format);

        Texture(size_t width, size_t height, TextureWrap wrap = TextureWrap::Clamp, TextureFilter filter = TextureFilter::Bilinear, TextureFormat format = TextureFormat::SRGBA, uint32_t mipLevels = 1,  void *data = nullptr);

        Texture(size_t width, size_t height, TextureWrap wrap = TextureWrap::Clamp, TextureFilter filter = TextureFilter::Bilinear, TextureFormat format = TextureFormat::SRGBA, uint32_t mipLevels = 1, const std::vector<void *>& data = {});

        Texture(size_t width, size_t height, TextureWrap wrap = TextureWrap::Clamp, TextureFilter filter = TextureFilter::Bilinear, TextureFormat format = TextureFormat::SRGBA, uint32_t mipLevels = 1, TextureType type = TextureType::Texture2D);

        ~Texture() = default;

        TextureHandle GetId() const ;

        size_t GetWidth() const;

        size_t GetHeight() const;

        void SetWrap(TextureWrap wrap);

        TextureWrap GetWrap();

    private:
        TextureHandle id{};
        size_t width{};
        size_t height{};
        TextureWrap wrap{};

    };
} // Rendering

#endif //MAXENGINE_GLTEXTURE_H
