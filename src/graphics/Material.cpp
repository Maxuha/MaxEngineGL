//
// Created by zykov on 3/19/2026.
//

#include "Material.h"

Material::Material(IShader *shader) : shader(shader) {
    // m_cpuBuffer.resize(16, 0);
    // m_ubo = new Rendering::UniformBuffer(m_cpuBuffer.data(), m_cpuBuffer.size());
}

MaterialId Material::GetId() const {
    return id;
}

IShader *Material::GetShader() const {
    return shader;
}

void Material::Enable() {
    // 3 - is material index
    // m_ubo->BindIndex(3);
    // shader->Enable();

    // for (const auto &property: properties) {
    //     std::visit([&](const auto &t) {
    //         shader->SetUniform(property.first, t);
    //     }, property.second);
    // }
    //
    // for (const auto &texture: textures) {
    //     shader->SetUniform(texture.first, *texture.second);
    // }
}

void Material::Disable() {
    shader->Disable();
}

std::unordered_map<std::string, UniformValue> Material::GetProperties() const {
    return properties;
}

void Material::SetUniform(const std::string &var, const float value, const bool isMaterial) {
    if (isMaterial) {
        auto [offset, size] = shader->GetProperty(var);
        memcpy(m_cpuBuffer.data() + offset, &value, sizeof(float));
    } else {
        properties[var] = value;
    }
    m_isDirty = true;
}

void Material::SetUniform(const std::string &var, const UniformValue &value, const bool isMaterial) {
    if (isMaterial) {
        auto [offset, size] = shader->GetProperty(var);
        memcpy(m_cpuBuffer.data() + offset, &value, sizeof(value.index()));

    } else {
        properties[var] = value;
    }
    m_isDirty = true;
}


void Material::SetUniform(const std::string &var, Rendering::IGLTexture &texture) {
    textures[var] = &texture;
}
