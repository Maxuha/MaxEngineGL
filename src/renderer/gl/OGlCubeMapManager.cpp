//
// Created by zykov on 10/20/2026.
//

#include "OGlCubeMapManager.h"

namespace Rendering {
    OGlCubeMapManager::OGlCubeMapManager() {
    }

    OGlCubeMapManager::~OGlCubeMapManager() {
    }

    CubeMapHandle OGlCubeMapManager::CreateCubeMap(const TextureCreateRequest &desc, const std::vector<void *> &datas) {
        const GLenum wrap = MapTextureWrap(desc.Wrap);
        const GLenum magFilter = MapTextureMagFilter(desc.Filter);
        GLenum minFilter;
        const bool useMipMaps = desc.useMipMaps;

        if (useMipMaps) {
            minFilter = MapTextureMipmapMinFilter(desc.Filter);
        } else {
            minFilter = MapTextureMinFilter(desc.Filter);
        }

        GLuint texture;
        glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &texture);

        glTextureParameteri(texture, GL_TEXTURE_WRAP_S, wrap);
        glTextureParameteri(texture, GL_TEXTURE_WRAP_T, wrap);
        glTextureParameteri(texture, GL_TEXTURE_WRAP_R, wrap);
        glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, minFilter);
        glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, magFilter);

        glTextureStorage2D(texture, desc.MipLevels, MapTextureInternalFormat(desc.Format), desc.Width, desc.Height);

        if (useMipMaps) {
            glGenerateTextureMipmap(texture);
        }

        for (int i = 0; i < datas.size(); ++i) {
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

        GLuint fbo;
        glCreateFramebuffers(1, &fbo);

        cubeMaps.push_back( {.glId = texture, .fbo = fbo} );

        return { .Id = cubeMaps.size() - 1 };
    }

    OGLCubeMapData & OGlCubeMapManager::GetCubeMap(const CubeMapHandle cubeMap) {
        return cubeMaps[cubeMap.Id];
    }
} // Rendering