//
// Created by zykov on 3/17/2026.
//

#ifndef MAXENGINE_DIRECTIONALLIGHT_H
#define MAXENGINE_DIRECTIONALLIGHT_H
#include "Light.h"


class DirectionalLight : public Light {
public:
    DirectionalLight(Color color, float intensity);

    void Enable(Shader &shader, int i) override;
};


#endif //MAXENGINE_DIRECTIONALLIGHT_H
