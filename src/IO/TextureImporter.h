//
// Created by zykov on 4/4/2026.
//

#ifndef MAXENGINE_TEXTUREINPORTER_H
#define MAXENGINE_TEXTUREINPORTER_H

#include <string>
#include "../renderer/domain/IGLTexture.h"
#include "model/TextureAsset.h"


class TextureImporter {
public:
    TextureAsset* Import(const std::string& fileName);
};



#endif //MAXENGINE_TEXTUREINPORTER_H
