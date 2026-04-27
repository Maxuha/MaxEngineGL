//
// Created by zykov on 4/4/2026.
//

#ifndef MAXENGINE_TEXTURE_H
#define MAXENGINE_TEXTURE_H

#include "glad/glad.h"


class Texture {
public:
    Texture(const unsigned char *data, int width, int height, const GLenum format);
    ~Texture();

    void Bind();
    void Unbind();

    void Activate(unsigned int i);

    int GetId() const {
        return Id;
    };

private:
    unsigned int Id{};

};



#endif //MAXENGINE_TEXTURE_H
