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
    Vector3 scale;

    Vector3 forward;
    Vector3 up;

    Vector3 Right() const;
    void Translate(Vector3 dir);
    void RotateYaw(float angle);
};


#endif //MAXENGINE_TRANSFORM_H