//
// Created by zykov on 4/22/2026.
//

#ifndef MAXENGINE_SPOTLIGHT_H
#define MAXENGINE_SPOTLIGHT_H
#include "Light.h"
#include "LocationalLight.h"


class SpotLight : public LocationalLight {
public:
    SpotLight(Color color, float intensity, float range, float innerAngle, float outerAngle);

    void Enable(Shader &shader, int i) override;

private:
    float range, innerAngle, outerAngle;
};


#endif //MAXENGINE_SPOTLIGHT_H
