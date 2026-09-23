//
// Created by zykov on 5/19/2026.
//

#ifndef MAXENGINE_ISHADER_H
#define MAXENGINE_ISHADER_H
#include <cstdint>
#include "../../math/Color.h"
#include "../../math/Matrix4x4.h"
#include <variant>

#include "../../utils/Hash.h"

enum class ShaderProperty;
class Color;
class Vector3;
class Matrix4x4;

struct ShaderId {
    uint64_t Id;

    bool operator==(const ShaderId &other) const {
        return Id == other.Id;
    }

    bool operator<(const ShaderId &other) const {
        return std::tie(Id) < std::tie(other.Id);
    }
};

template<>
struct std::hash<ShaderId> {
    size_t operator()(const ShaderId &s) const noexcept {
        return Hash::Generate(std::to_string(s.Id));
    }
};

using UniformValue = std::variant<int, float, Vector3, Matrix4x4, Color>;

struct ShaderMetaProperty {
    size_t offset;
    size_t size;
    UniformValue value;
};

enum class ShaderProperty {
    COLOR_DIFFUSE,
    COLOR_SPECULAR,
    COLOR_EMISSIVE,
    COLOR_AMBIENT,
    COLOR_TRANSPARENT,
    COLOR_REFLECTIVE,
    COLOR_NORMAL,
    SHININESS,
    COLOR_OPACITY
};

class IShader {
public:
    virtual ~IShader() = default;

    virtual void AddProperty(ShaderProperty shaderProperty, ShaderMetaProperty shaderPropertyMeta) = 0;

    virtual ShaderMetaProperty GetProperty(ShaderProperty name) const = 0;

    virtual std::unordered_map<ShaderProperty, ShaderMetaProperty>& GetProperties() = 0;

};


#endif //MAXENGINE_ISHADER_H
