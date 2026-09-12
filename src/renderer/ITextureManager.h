//
// Created by zykov on 6/26/2026.
//

#ifndef MAXENGINE_ITEXTUREMANAGER_H
#define MAXENGINE_ITEXTUREMANAGER_H
#include <cstdint>

namespace Rendering {
    struct TextureHandle { uint32_t Id = 0; };
    enum class TextureFormat;

    class ITextureManager {
    public:
        virtual ~ITextureManager() = default;

        virtual TextureHandle CreateTexture(size_t width, size_t height, TextureFormat format, void* data) = 0;
    };
} // Rendering

#endif //MAXENGINE_ITEXTUREMANAGER_H
