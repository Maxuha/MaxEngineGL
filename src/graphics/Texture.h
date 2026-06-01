//
// Created by zykov on 4/4/2026.
//

#ifndef MAXENGINE_TEXTURE_H
#define MAXENGINE_TEXTURE_H

#include <string>
#include <tuple>

#include "ITexture.h"
#include "../utils/Hash.h"
#include "glad/glad.h"

struct TextureId {
    uint32_t Id;

    bool operator==(const TextureId &other) const {
        return Id == other.Id;
    }

    bool operator<(const TextureId &other) const {
        return std::tie(Id) < std::tie(other.Id);
    }
};

template<>
struct std::hash<TextureId> {
    size_t operator()(const TextureId &s) const noexcept {
        return Hash::Generate(std::to_string(s.Id));
    }
};

class Texture : public ITexture{
public:
    Texture(TextureId textureId, const unsigned char *data, int width, int height, GLenum format);
    ~Texture() override;

    void Bind() override;
    void Unbind() override;

    void Activate(unsigned int i) override;

    TextureId GetId() const {
        return textureId;
    };

private:
    unsigned int Id{};
    TextureId textureId;

};



#endif //MAXENGINE_TEXTURE_H
