//
// Created by zykov on 3/19/2026.
//

#include "Material.h"

#include "Cubemap.h"
#include "../RenderContext.h"
#include "../../di/DIContainer.h"
#include "../../math/Vertex.h"

Material::Material(const MaterialHandle handle) : id(handle), shader(nullptr) {
}

Material::Material(const Shader *shader, const size_t bufferSize, const std::string &name) : name(name), shader(nullptr) {
    cpuBuffer.resize(bufferSize, 0);

    Rendering::MaterialDesc desc;
    desc.bufferSize = bufferSize;

    id = DIContainer::GetInstance().Get<Rendering::IRenderer>()->CreateMaterial(shader->GetHandle(), desc);

    isDirty = true;
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

void Material::SetTexture(const uint32_t slot, const Rendering::Texture &texture) const {
    DIContainer::GetInstance().Get<Rendering::IRenderer>()->BindTextureToMaterial(*this, slot, texture);
}

void Material::SetCubemap(const uint32_t slot, const Rendering::Cubemap &texture) const {
    DIContainer::GetInstance().Get<Rendering::IRenderer>()->BindCubemapToMaterial(*this, slot, texture);
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
