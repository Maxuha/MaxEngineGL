//
// Created by zykov on 4/4/2026.
//

#define STB_IMAGE_IMPLEMENTATION

#include "TextureImporter.h"
#include "stb_image.h"

Texture TextureImporter::Import(const char* fileName) {
    int width, height, nrChannels;
    const unsigned char *data = stbi_load(fileName, &width, &height, &nrChannels, 4);
    const Texture texture {
        data, width, height
    };
    return texture;
}
