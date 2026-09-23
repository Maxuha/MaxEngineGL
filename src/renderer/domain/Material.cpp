//
// Created by zykov on 3/19/2026.
//

#include "Material.h"

#include "../RenderContext.h"
#include "../../di/DIContainer.h"
#include "../../math/Vertex.h"

Material::Material(const MaterialHandle handle) : id(handle), shader(nullptr) {
}

Material::Material(const Shader *shader, const std::string &name) : name(name), shader(nullptr) {
    cpuBuffer.resize(32, 0);

    Rendering::MaterialDesc desc;

    Rendering::ResourceBinding resourceBinding{};
    resourceBinding.slot = 2;
    resourceBinding.binding = 4;
    resourceBinding.type = Rendering::ResourceType::SampledImage;

    Rendering::ResourceBinding resourceBinding2{};
    resourceBinding2.slot = 2;
    resourceBinding2.binding = 5;
    resourceBinding2.type = Rendering::ResourceType::SampledImage;

    Rendering::ResourceBinding resourceBinding4{};
    resourceBinding4.slot = 2;
    resourceBinding4.binding = 6;
    resourceBinding4.type = Rendering::ResourceType::SampledImage;

    Rendering::ResourceBinding resourceBinding3{};
    resourceBinding3.slot = 2;
    resourceBinding3.binding = 3;
    resourceBinding3.type = Rendering::ResourceType::UniformBuffer;

    desc.resourceSet.bindings.push_back(resourceBinding);
    desc.resourceSet.bindings.push_back(resourceBinding2);
    desc.resourceSet.bindings.push_back(resourceBinding4);
    desc.resourceSet.bindings.push_back(resourceBinding3);

    id = DIContainer::GetInstance().Get<Rendering::IRenderer>()->CreateMaterial(shader->GetHandle(), desc);

    dirtyFrames = 3;
}

MaterialHandle Material::GetId() const {
    return id;
}

IShader *Material::GetShader() const {
    return shader;
}

bool Material::IsDirty() const {
    return isDirty;
}

void Material::ClearDirty() {
    isDirty = false;
}

void Material::SetTexture(const uint32_t slot, Rendering::Texture &texture) {
    DIContainer::GetInstance().Get<Rendering::IRenderer>()->BindTextureToMaterial(*this, slot, texture);
}

MaterialHandle Material::GetMaterialHandle() const {
    return id;
}

std::string Material::GetName() const {
    return name;
}

void Material::Apply(const std::string &slot)  {
    if (const auto it = propertiesMeta.find(slot); it != propertiesMeta.end()) {
        const ShaderMetaProperty& meta = it->second;
    }
}

void Material::Apply(const ShaderProperty &slot) {
    if (const auto it = commonPropertiesMeta.find(slot); it != commonPropertiesMeta.end()) {
        const ShaderMetaProperty& meta = it->second;
    }
}
