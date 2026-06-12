//
// Created by zykov on 3/17/2026.
//

#ifndef MAXENGINE_LIGHT_H
#define MAXENGINE_LIGHT_H
#include "../../components/Component.h"
#include "../../math/Color.h"


class Shader;

class Light : public Component {
public:
    float intensity = 1.0f;
    Color color = Color(1, 1, 1, 1);

    Light();
};


#endif //MAXENGINE_LIGHT_H
