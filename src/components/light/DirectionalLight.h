//
// Created by zykov on 3/17/2026.
//

#ifndef MAXENGINE_DIRECTIONALLIGHT_H
#define MAXENGINE_DIRECTIONALLIGHT_H
#include "Light.h"


class DirectionalLight : public Light {
public:
    DirectionalLight();

    void Update(float delta_time) override;
};


#endif //MAXENGINE_DIRECTIONALLIGHT_H
