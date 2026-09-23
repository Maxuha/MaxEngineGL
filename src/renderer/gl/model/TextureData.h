//
// Created by zykov on 5/21/2026.
//

#ifndef MAXENGINE_TEXTUREDESC_H
#define MAXENGINE_TEXTUREDESC_H
#include <cstdint>
#include "../../ITextureManager.h"


namespace Rendering {
    enum class TextureFormat {
        RGB,
        RGBA,
        SRGBA,
        D32_FLOAT,
        DEPTH
    };

    enum class TextureWrap : std::uint8_t {
        Repeat,
        MirroredRepeat,
        ClampToEdge,
        ClampToBorder
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
}


#endif //MAXENGINE_TEXTUREDESC_H
