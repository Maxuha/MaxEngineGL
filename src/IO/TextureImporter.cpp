//
// Created by zykov on 4/4/2026.
//

#define STB_IMAGE_IMPLEMENTATION

#include "TextureImporter.h"
#include <string>
#include "stb_image.h"

Texture* TextureImporter::Import(const std::string& fileName) {
    int width {}, height {}, nrChannels {};
    unsigned char *data = stbi_load(fileName.c_str(), &width, &height, &nrChannels, 4);
    auto* texture = new Texture(data, width, height);
    stbi_image_free(data);
    return texture;
}
