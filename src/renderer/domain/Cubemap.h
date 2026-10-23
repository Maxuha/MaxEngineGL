//
// Created by zykov on 10/20/2026.
//

#ifndef MAXENGINE_CUBEMAP_H
#define MAXENGINE_CUBEMAP_H

#include <vector>
#include "../gl/model/TextureData.h"

namespace Rendering {
    struct CubeMapHandle {
        uint32_t Id = 0;
    };

    class Cubemap {
    public:
        Cubemap(const size_t size, const TextureFormat format, const std::vector<void*>& data);
        virtual ~Cubemap() = default;

        CubeMapHandle GetHandle() const;

    private:
        CubeMapHandle handle;
    };
} // Rendering

#endif //MAXENGINE_CUBEMAP_H
