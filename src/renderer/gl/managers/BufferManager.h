//
// Created by zykov on 6/12/2026.
//

#ifndef MAXENGINE_BUFFERMANAGER_H
#define MAXENGINE_BUFFERMANAGER_H
#include <vector>

#include "BufferData.h"

namespace Rendering {
    class BufferManager {
    public:
        void CreateBuffer(const BufferDesc &desc, const void *data = nullptr);
        BufferHandle CreateFrameBuffer(const TextureData& textureData);

    private:
        std::vector<BufferData> buffers;
        std::vector<FrameBufferData> frameBuffers;
    };
} // Rendering

#endif //MAXENGINE_BUFFERMANAGER_H
