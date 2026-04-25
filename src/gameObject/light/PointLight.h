//
// Created by zykov on 4/22/2026.
//

#ifndef MAXENGINE_POINTLIGHT_H
#define MAXENGINE_POINTLIGHT_H
#include "Light.h"
#include "LocationalLight.h"
#include "../../math/Color.h"


class PointLight : public LocationalLight {
public:
    explicit PointLight(const Color &color, float intensity, float range);

    void Enable(Shader &shader, int i) override;

private:
    float range;
};


#endif //MAXENGINE_POINTLIGHT_H
