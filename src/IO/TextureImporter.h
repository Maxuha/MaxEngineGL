//
// Created by zykov on 4/4/2026.
//

#ifndef MAXENGINE_TEXTUREINPORTER_H
#define MAXENGINE_TEXTUREINPORTER_H

#include <string>
#include "../graphics/Texture.h"


class TextureImporter {
public:
    Texture* Import(const std::string& fileName);
};



#endif //MAXENGINE_TEXTUREINPORTER_H
