//
// Created by zykov on 4/22/2026.
//

#ifndef MAXENGINE_SPOTLIGHT_H
#define MAXENGINE_SPOTLIGHT_H
#include "LocationalLight.h"


class SpotLight : public Light {
public:
    float GetInnerAngle() const;
    float GetOuterAngle() const;
    float GetRange() const;

private:
    // swap inner and outer
    float range = 2, innerAngle = 1.0, outerAngle = 0.5;
};


#endif //MAXENGINE_SPOTLIGHT_H
