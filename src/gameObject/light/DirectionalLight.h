//
// Created by zykov on 3/17/2026.
//

#ifndef MAXENGINE_DIRECTIONALLIGHT_H
#define MAXENGINE_DIRECTIONALLIGHT_H
#include "Light.h"
#include "../../components/Transform.h"
#include "../../math/Color.h"
#include "../../math/Vector3.h"


class DirectionalLight : public Light {
    public:
    DirectionalLight() {
        AddComponent<Transform>();
    }

    float intensity = 0;;
    Vector3 direction = {};
    Color color = {} ;
};


#endif //MAXENGINE_DIRECTIONALLIGHT_H