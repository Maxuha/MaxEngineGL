//
// Created by zykov on 5/19/2026.
//

#ifndef MAXENGINE_ISHADER_H
#define MAXENGINE_ISHADER_H
#include <cstdint>

#include "Texture.h"
#include "../renderer/domain/IPipeline.h"
#include "../utils/Hash.cpp"

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

struct ShaderProperty {
    size_t offset;
    size_t size;
};

class IShader {
public:
    virtual ~IShader() = default;

    [[nodiscard]] virtual ShaderId GetId() const = 0;

    virtual void Enable() const = 0;

    virtual void Disable() const = 0;

    virtual void AddProperty(const std::string &name, ShaderProperty property) = 0;

    [[nodiscard]] virtual ShaderProperty GetProperty(const std::string &name) const = 0;

    virtual void SetUniform(const std::string &var, Matrix4x4 mat) const = 0;

    virtual  void SetUniform(const std::string &var, Vector3 vec) const = 0;

    virtual  void SetUniform(const std::string &var, Texture& vec) const = 0;

    virtual  void SetUniform(const std::string &var, int val) const = 0;

    virtual void SetUniform(const std::string &var, float val) const = 0;

    virtual void SetUniform(const std::string &var, Color color) const = 0;



};


#endif //MAXENGINE_ISHADER_H
