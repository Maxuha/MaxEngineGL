//
// Created by zykov on 4/4/2026.
//

#include "Texture.h"
#include "glad/glad.h"

Texture::Texture(const unsigned char *data, const int width, const int height) {
    glGenTextures(1, &Id);

    Bind();

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

}

void Texture::Bind() {
    glBindTexture(GL_TEXTURE_2D, Id);
}

void Texture::Unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}
