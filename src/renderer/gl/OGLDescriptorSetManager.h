//
// Created by zykov on 8/8/2026.
//

#ifndef MAXENGINE_OGLDESCRIPTORSETMANAGER_H
#define MAXENGINE_OGLDESCRIPTORSETMANAGER_H
#include "CommandBuffer.h"
#include "../IResourceSetManager.h"
#include "model/OGLPipelineLayout.h"

namespace Rendering {
    struct DescriptorLayout;
    struct DescriptorPoolHandle;

    struct OGLResourceItem {
        BufferHandle bufferHandle = BufferHandle { .Id = 9999999 };
        TextureHandle textureHandle = TextureHandle { .Id = 9999999 };
        uint32_t binding = 0;

        bool IsBuffer() const {
            return bufferHandle.Id != 9999999;
        }
    };

    struct OGLResourceSet {
        std::vector<OGLResourceItem> items;
    };

    class OGLDescriptorSetManager : public IResourceSetManager {
    public:
        explicit OGLDescriptorSetManager();

        ~OGLDescriptorSetManager() override;

        ResourceSetHandle CreateDescriptorSet(const OGLDescriptorSetLayout& layout);

        void BindBuffer(ResourceSetHandle setHandle, uint32_t slot, BufferHandle bufferHandle) override;

        void BindTexture(ResourceSetHandle setHandle, uint32_t slot, TextureHandle textureHandle) override;

        OGLResourceSet& GetResourceSet(ResourceSetHandle setHandle);

    private:
        std::vector<OGLResourceSet> resourceSets;
    };
} // Rendering

#endif //MAXENGINE_OGLDESCRIPTORSETMANAGER_H
