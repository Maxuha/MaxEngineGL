//
// Created by zykov on 5/28/2026.
//

#ifndef MAXENGINE_TEXTUREASSET_H
#define MAXENGINE_TEXTUREASSET_H


struct TextureAsset {
    size_t Width;
    size_t Height;
    void* Data;
    Rendering::TextureWrap Wrap;
};


#endif //MAXENGINE_TEXTUREASSET_H
