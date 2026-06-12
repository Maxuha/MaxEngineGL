//
// Created by zykov on 5/12/2026.
//

#ifndef MAXENGINE_GLTEXTURE_H
#define MAXENGINE_GLTEXTURE_H
#include <cstdint>
#include <string>
#include "../domain/IGLTexture.h"

namespace Rendering {
    class GLTexture : public IGLTexture {
    public:

        GLTexture(size_t width, size_t height);

        GLTexture(size_t width, size_t height, TextureWrap wrap, void *data);

        ~GLTexture() override = default;

        TextureHandle GetId() override;

        size_t GetWidth() const;

        size_t GetHeight() const;

        void SetWrap(TextureWrap wrap) override;

        TextureWrap GetWrap() override;

    private:
        TextureHandle id{};
        size_t width{};
        size_t height{};
        TextureWrap wrap{};

    };
} // Rendering

#endif //MAXENGINE_GLTEXTURE_H
