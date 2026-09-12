//
// Created by zykov on 6/21/2026.
//

#ifndef MAXENGINE_IVULKANTEXTUREMANAGER_H
#define MAXENGINE_IVULKANTEXTUREMANAGER_H

namespace Rendering {
    struct TextureHandle;
    struct VulkanTexture;

    class IVulkanTextureManager {
        public:
        virtual ~IVulkanTextureManager() = default;

        VulkanTexture GetTexture(TextureHandle texture);
    };
} // Rendering

#endif //MAXENGINE_IVULKANTEXTUREMANAGER_H
