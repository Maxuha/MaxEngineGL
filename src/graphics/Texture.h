//
// Created by zykov on 4/4/2026.
//

#ifndef MAXENGINE_TEXTURE_H
#define MAXENGINE_TEXTURE_H


class Texture {
public:
    Texture(const unsigned char *data, int width, int height);
    ~Texture();

    void Bind();
    void Unbind();

private:
    unsigned int Id{};

};



#endif //MAXENGINE_TEXTURE_H
