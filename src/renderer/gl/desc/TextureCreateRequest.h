//
// Created by zykov on 5/28/2026.
//

#ifndef MAXENGINE_TEXTURECREATEREQUEST_H
#define MAXENGINE_TEXTURECREATEREQUEST_H

#include <cstdint>

namespace Rendering {
    struct TextureCreateRequest {

        size_t Width{};
        size_t Height{};
        size_t MipLevels{};
        size_t ArraySize{};
        TextureFormat Format{};
    };
} // Rendering

#endif //MAXENGINE_TEXTURECREATEREQUEST_H
