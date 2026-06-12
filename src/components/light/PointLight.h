//
// Created by zykov on 4/22/2026.
//

#ifndef MAXENGINE_POINTLIGHT_H
#define MAXENGINE_POINTLIGHT_H
#include "LocationalLight.h"


class PointLight : public Light {
public:
    float GetRange() const;

    void SetRange(float range);

private:
    float range = 1.0f;
};


#endif //MAXENGINE_POINTLIGHT_H
