//
// Created by zykov on 8/8/2026.
//

#include "OGLDescriptorSetManager.h"

namespace Rendering {
    OGLDescriptorSetManager::OGLDescriptorSetManager() {
    }

    OGLDescriptorSetManager::~OGLDescriptorSetManager() {

    }

    ResourceSetHandle OGLDescriptorSetManager::CreateDescriptorSet(const OGLDescriptorSetLayout &layout) {
        const auto setHandle = ResourceSetHandle { .id = resourceSets.size() };
        OGLResourceSet resourceSet;
        resourceSet.items.resize(layout.bindings.size());
        for (int i = 0; i < layout.bindings.size(); i++) {
            resourceSet.items[i].binding = layout.bindings[i].binding_slot;
        }
        resourceSets.push_back(resourceSet);
        return setHandle;
    }

    void OGLDescriptorSetManager::BindBuffer(const ResourceSetHandle setHandle, const uint32_t slot, const BufferHandle bufferHandle) {
        OGLResourceSet& resourceSet = resourceSets[setHandle.id];
        const auto item = std::ranges::find_if(resourceSet.items, [&](const OGLResourceItem& item) { return item.binding == slot;});
        if (item == resourceSet.items.end()) {
            return;
        }
        item->bufferHandle = bufferHandle;
    }

    void OGLDescriptorSetManager::BindTexture(const ResourceSetHandle setHandle, const uint32_t slot, const TextureHandle textureHandle) {
        OGLResourceSet& resourceSet = resourceSets[setHandle.id];
        const auto item = std::ranges::find_if(resourceSet.items, [&](const OGLResourceItem& item) { return  item.binding == slot;});
        if (item == resourceSet.items.end()) {
            return;
        }
        item->textureHandle = textureHandle;
    }

    OGLResourceSet & OGLDescriptorSetManager::GetResourceSet(const ResourceSetHandle setHandle) {
        return resourceSets[setHandle.id];
    }
} // Rendering