//
// Created by zykov on 3/17/2026.
//

#ifndef MAXENGINE_DIRECTIONALLIGHT_H
#define MAXENGINE_DIRECTIONALLIGHT_H
#include "Light.h"
#include "../../components/Transform.h"
#include "../../math/Vector3.h"


class DirectionalLight : public Light {
    public:
    DirectionalLight() {
        AddComponent<Transform>();
        intensity = 10.0f;
        direction = Vector3(-0.2f, -1.0f, -1.0f);
        color = Vector3(1.0f, 1.0f, 1.0f);
    }

    float intensity;
    Vector3 direction;
    Vector3 color;
};


#endif //MAXENGINE_DIRECTIONALLIGHT_H