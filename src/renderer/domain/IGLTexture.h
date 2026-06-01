//
// Created by zykov on 5/24/2026.
//

#ifndef MAXENGINE_IGLTEXTURE_H
#define MAXENGINE_IGLTEXTURE_H
#include "../gl/model/TextureData.h"

namespace Rendering {

    class IGLTexture {
    public:
        virtual ~IGLTexture() = default;

        virtual TextureId GetId() = 0;

        virtual void SetWrap(TextureWrap wrap) = 0;

        virtual TextureWrap GetWrap() = 0;
    };
} // Rendering

#endif //MAXENGINE_IGLTEXTURE_H
