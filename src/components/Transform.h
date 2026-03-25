//
// Created by zykov on 3/15/2026.
//

#ifndef MAXENGINE_TRANSFORM_H
#define MAXENGINE_TRANSFORM_H
#include "Component.h"
#include "../math/Vector3.h"

class Transform : public Component {
    public:
    Vector3 position;
    Vector3 rotation;
    Vector3 scale = { 1.0f, 1.0f, 1.0f };

    Vector3 right { Vector3::Right() };
    Vector3 forward { Vector3::Forward() };
    Vector3 up { Vector3::Up() };

    Vector3 pivot = { 0.5f, 0.5f, 0.5f };

    void Translate(Vector3 dir);
    void RotateYaw(float angle);
    void RotatePitch(float angle);
};


#endif //MAXENGINE_TRANSFORM_H