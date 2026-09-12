//
// Created by zykov on 5/12/2026.
//

#ifndef MAXENGINE_GLTEXTURE_H
#define MAXENGINE_GLTEXTURE_H
#include <cstdint>
#include <string>
#include "../domain/IGLTexture.h"

namespace Rendering {
    class Texture {
    public:

        Texture(size_t width, size_t height);

        Texture(size_t width, size_t height, TextureWrap wrap, void *data);

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
