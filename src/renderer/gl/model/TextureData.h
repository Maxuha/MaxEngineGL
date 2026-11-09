//
// Created by zykov on 5/21/2026.
//

#ifndef MAXENGINE_TEXTUREDESC_H
#define MAXENGINE_TEXTUREDESC_H


namespace Rendering {
    enum class TextureFormat {
        RGB,
        RGBA,
        SRGBA,
        DEPTH,
        UNDEFINED
    };

    enum class TextureType {
        Texture2D,
        Cube,
    };

    enum class TextureWrap : std::uint8_t {
        Repeat,
        Clamp,
        Mirror,
        MirrorOnce,
    };

    enum class TextureFilter : std::uint8_t {
        Point,
        Bilinear,
        Trilinear,
    };

    struct TextureCreateRequest {
        size_t Width{};
        size_t Height{};
        TextureFormat Format = TextureFormat::RGB;
        TextureWrap Wrap = TextureWrap::Repeat;
        TextureFilter Filter = TextureFilter::Bilinear;
        TextureType Type = TextureType::Texture2D;
        bool useMipMaps = false;
        size_t MipLevels = 1;
    };

    struct TextureData {
        uint32_t id = {};
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t mipLevels = 1;
        TextureFormat format = TextureFormat::SRGBA;
        TextureWrap wrap = TextureWrap::Repeat;
        bool isRenderTarget = false;
    };

    struct CubeMapCreateRequest {
        size_t Size;
        TextureFormat Format;
    };
}


#endif //MAXENGINE_TEXTUREDESC_H
