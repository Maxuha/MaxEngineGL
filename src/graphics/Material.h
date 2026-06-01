//
// Created by zykov on 3/19/2026.
//

#ifndef MAXENGINE_MATERIAL_H
#define MAXENGINE_MATERIAL_H
#include <any>
#include <variant>

#include "Shader.h"
#include "Texture.h"
#include "../gameObject/Camera.h"
#include "../renderer/domain/IGLTexture.h"


class GameObject;

struct MaterialId {
    unsigned int Id;

    bool operator==(const MaterialId &other) const {
        return Id == other.Id;
    }

    bool operator<(const MaterialId &other) const {
        return std::tie(Id) < std::tie(other.Id);
    }
};

template<>
struct std::hash<MaterialId> {
    size_t operator()(const MaterialId &s) const noexcept {
        return Hash::Generate(std::to_string(s.Id));
    }
};

using UniformValue = std::variant<int, float, Vector3, Matrix4x4>;

class Material {
public:
    explicit Material(IShader *shader);

    virtual ~Material() = default;

    MaterialId GetId() const;

    IShader *GetShader() const;

    virtual void Enable();

    virtual void Disable();

    template<typename T>
    void SetProperty(const std::string &slot, T value) {
        // auto it = propertiesLayout.find(slot);
        // if (it != propertiesLayout.end()) {
        //     memcpy(m_cpuBuffer.data() + it->second.offset, &value, sizeof(T));
        //
        //     m_isDirty = true;
        // }
    }

    std::unordered_map<std::string, UniformValue> GetProperties() const;

    std::any GetProperty(const std::string &slot) const {
        return shader->GetProperty(slot);
    }

    void SetTexture(const std::string &slot, Rendering::IGLTexture &texture) {
        textures[slot] = &texture;
    }

    Rendering::IGLTexture* GetTexture(const std::string &slot) const {
        return textures.at(slot);
    }

    std::vector<uint8_t> GetBufferData() const {
        return m_cpuBuffer;
    }

    void SetUniform(const std::string &var, float value, bool isMaterial);

    void SetUniform(const std::string &var, const UniformValue &value, bool isMaterial);

    void SetUniform(const std::string &var, Rendering::IGLTexture &texture);

private:
    MaterialId id{};
    std::unordered_map<std::string, Rendering::IGLTexture*> textures;
    std::unordered_map<std::string, UniformValue> properties;
    std::vector<uint8_t> m_cpuBuffer;
    bool m_isDirty = true;

protected:
    IShader *shader;
};


#endif //MAXENGINE_MATERIAL_H
