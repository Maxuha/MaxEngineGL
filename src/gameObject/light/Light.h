//
// Created by zykov on 3/17/2026.
//

#ifndef MAXENGINE_LIGHT_H
#define MAXENGINE_LIGHT_H
#include "../GameObject.h"
#include "../../math/Color.h"


class Shader;

class Light : public GameObject {
public:
    float intensity = 1.0f;
    Color color;

    Light(Color color, float intensity);

    virtual void Enable(Shader &shader, int i) = 0;
};


#endif //MAXENGINE_LIGHT_H
