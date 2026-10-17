//
// Created by zykov on 10/15/2026.
//

#ifndef MAXENGINE_PLANE_H
#define MAXENGINE_PLANE_H
#include "../GameObject.h"


class Plane : public GameObject {
public:
    static Plane* BuildPlane();

    Transform* transform;

    void Start() override;

    void Update(float delta_time) override;
};


#endif //MAXENGINE_PLANE_H
