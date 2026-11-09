//
// Created by zykov on 10/20/2026.
//

#include "Cubemap.h"

#include "../../di/DIContainer.h"


namespace Rendering {
    Cubemap::Cubemap(const size_t size, const TextureFormat format, const std::vector<void*>& data) {
        TextureCreateRequest request{};
        request.Width = size;
        request.Height = size;
        request.Format = format;
        request.MipLevels = 1;
        request.Wrap = TextureWrap::Clamp;
        request.Filter = TextureFilter::Bilinear;
        request.useMipMaps = false;
        request.Type = TextureType::Cube;
        handle = DIContainer::GetInstance().Get<IRenderer>()->CreateCubeMap(request, data);
    }

    CubeMapHandle Cubemap::GetHandle() const {
        return handle;
    }
} // Rendering