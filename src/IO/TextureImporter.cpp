//
// Created by zykov on 4/4/2026.
//

#define STB_IMAGE_IMPLEMENTATION

#include "TextureImporter.h"
#include <string>
#include "stb_image.h"
#include "../di/DIContainer.h"

TextureAsset* TextureImporter::Import(const std::string& fileName) {
    int width {}, height {}, nrChannels {};
    unsigned char *data = stbi_load(fileName.c_str(), &width, &height, &nrChannels, 4);

    Rendering::TextureData desc;
    desc.width = static_cast<uint32_t>(width);
    desc.height = static_cast<uint32_t>(height);
    desc.format = Rendering::TextureFormat::RGBA;
    desc.mipLevels = 0;

  //  auto textureData = DIContainer::GetInstance().Get<Rendering::IRenderer>()->CreateTexture(data, width, height);
    // stbi_image_free(data);
   // return new Rendering::GLTexture(textureData);

    auto* textureAsset = new TextureAsset();
    textureAsset->Width = desc.width;
    textureAsset->Height = desc.height;
    textureAsset->Data = data;
    return textureAsset;
}
