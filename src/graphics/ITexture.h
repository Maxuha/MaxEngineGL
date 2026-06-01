//
// Created by zykov on 5/19/2026.
//

#ifndef MAXENGINE_ITEXTURE_H
#define MAXENGINE_ITEXTURE_H


class ITexture {
public:
    virtual ~ITexture() = default;
    virtual void Bind() = 0;
    virtual void Unbind() = 0;
    virtual void Activate(unsigned int i) = 0;
};


#endif //MAXENGINE_ITEXTURE_H
