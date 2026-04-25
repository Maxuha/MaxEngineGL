//
// Created by zykov on 4/25/2026.
//

#ifndef MAXENGINE_LOCATIONALLIGHT_H
#define MAXENGINE_LOCATIONALLIGHT_H
#include "Light.h"


class LocationalLight : public Light {
public:
    LocationalLight(const Color &color, float intensity)
        : Light(color, intensity) {
    }

protected:
    float constant = 1.0f, linear = 0.09f, quadratic = 0.032f;
};


#endif //MAXENGINE_LOCATIONALLIGHT_H
