//
// Created by zykov on 6/26/2026.
//

#ifndef MAXENGINE_OGLTEXTUREMANAGER_H
#define MAXENGINE_OGLTEXTUREMANAGER_H

#include "../../renderer/ITextureManager.h"


namespace Rendering {
    struct OGLTexture {
        uint32_t Id;
    };

    class OGLTextureManager : public ITextureManager {
    public:
        explicit OGLTextureManager();
        ~OGLTextureManager() override;

        TextureHandle CreateTexture(size_t width, size_t height, TextureFormat format, void* data) override;

        OGLTexture GetTexture(TextureHandle texture) const;

    private:
        std::vector<OGLTexture> textures;

        GLenum MapTextureInternalFormat(TextureFormat format);
        GLenum MapTextureExternalFormat(TextureFormat format);
    };
} // Rendering

#endif //MAXENGINE_OGLTEXTUREMANAGER_H
