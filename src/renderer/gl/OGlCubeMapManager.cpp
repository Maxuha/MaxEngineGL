//
// Created by zykov on 10/20/2026.
//

#include "OGlCubeMapManager.h"

#include <format>

namespace Rendering {
    OGlCubeMapManager::OGlCubeMapManager() {
    }

    OGlCubeMapManager::~OGlCubeMapManager() {
    }

    CubeMapHandle OGlCubeMapManager::CreateCubeMap(const TextureCreateRequest &desc, const std::vector<void *> &datas) {
        GLuint texture;
        glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &texture);

        glTextureStorage2D(texture, 1, MapTextureInternalFormat(desc.Format), desc.Width, desc.Height);

        glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(texture, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        for (int i = 0; i < 6; ++i) {
            if (datas[i]) {
                glTextureSubImage3D(
                    texture,
                    0,
                    0, 0, i,
                    desc.Width, desc.Height, 1,
                    MapTextureExternalFormat(desc.Format),
                    GL_UNSIGNED_BYTE,
                    datas[i]
                );
            }
        }

        cubeMaps.push_back( {.glId = texture} );

        return { .Id = cubeMaps.size() - 1 };
    }

    OGLCubeMapData & OGlCubeMapManager::GetCubeMap(const CubeMapHandle cubeMap) {
        return cubeMaps[cubeMap.Id];
    }
} // Rendering