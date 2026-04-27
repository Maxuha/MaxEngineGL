//
// Created by zykov on 4/4/2026.
//

#include "Texture.h"
#include "glad/glad.h"

Texture::Texture(const unsigned char *data, const int width, const int height, const GLenum format) {
    glGenTextures(1, &Id);

    glBindTexture(GL_TEXTURE_2D, Id);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() {
    if (Id != 0) {
        glDeleteTextures(1, &Id);
    }
}

void Texture::Bind() {
    glBindTexture(GL_TEXTURE_2D, Id);
}

void Texture::Unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Activate(const unsigned int i) {
    glActiveTexture(GL_TEXTURE0 + i);
}
