//
// Created by zykov on 3/19/2026.
//

#ifndef MAXENGINE_MATERIAL_H
#define MAXENGINE_MATERIAL_H
#include <any>
#include <variant>

#include "Shader.h"
#include "../../gameObject/Camera.h"
#include "IGLTexture.h"
#include "struct/BufferHandle.h"


namespace Rendering {
    class IRenderer;
}

class GameObject;

struct MaterialHandle {
    uint32_t Id;

    bool operator==(const MaterialHandle &other) const {
        return Id == other.Id;
    }

    bool operator<(const MaterialHandle &other) const {
        return std::tie(Id) < std::tie(other.Id);
    }
};

template<>
struct std::hash<MaterialHandle> {
    size_t operator()(const MaterialHandle &s) const noexcept {
        return Hash::Generate(std::to_string(s.Id));
    }
};


class Material {
public:
    explicit Material(IShader *shader, std::string name = "New Material");

    virtual ~Material() = default;

    MaterialHandle GetId() const;

    IShader *GetShader() const;

    template<typename T>
    void SetProperty(const std::string &slot, T value) {

        //TODO Remove
        propertiesMeta["material.shininess"] = { 0, sizeof(float) };
        propertiesMeta["material.diffuseColor"] = { 16, sizeof(Vector4) };

        auto it = propertiesMeta.find(slot);
        if (it != propertiesMeta.end()) {
            const ShaderMetaProperty& meta = it->second;

            memcpy(cpuBuffer.data() + meta.offset, &value, meta.size);
            Apply(slot);
        }
    }

    template<typename T>
    void SetProperty(const ShaderProperty &slot, T value) {

        //TODO Remove
        commonPropertiesMeta[ShaderProperty::SHININESS] = { 0, sizeof(float) };
        commonPropertiesMeta[ShaderProperty::COLOR_DIFFUSE] = { 16, sizeof(Vector4) };

        auto it = commonPropertiesMeta.find(slot);
        if (it != commonPropertiesMeta.end()) {
            const ShaderMetaProperty& meta = it->second;

            memcpy(cpuBuffer.data() + meta.offset, &value, meta.size);
            Apply(slot);
        }
    }

    std::any GetProperty(const ShaderProperty &slot) const {
        return shader->GetProperty(slot);
    }

    void SetTexture(const std::string &slot, Rendering::IGLTexture &texture) {
        textures[slot] = &texture;
    }

    Rendering::IGLTexture* GetTexture(const std::string &slot) const {
        return textures.at(slot);
    }

    std::vector<uint8_t> GetBufferData() const {
        return cpuBuffer;
    }

private:
    MaterialHandle id{};
    std::unordered_map<std::string, Rendering::IGLTexture*> textures;
    //std::unordered_map<std::string, UniformValue> properties;
    std::unordered_map<std::string, ShaderMetaProperty> propertiesMeta;
    //std::unordered_map<ShaderProperty, UniformValue> commonProperties;
    std::unordered_map<ShaderProperty, ShaderMetaProperty> commonPropertiesMeta;
    std::vector<uint8_t> cpuBuffer;
    std::string name;

    void Apply(const std::string& slot);

    void Apply(const ShaderProperty& slot);

protected:
    IShader *shader;
};


#endif //MAXENGINE_MATERIAL_H
