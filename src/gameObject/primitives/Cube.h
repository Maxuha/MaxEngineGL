//
// Created by zykov on 3/15/2026.
//

#ifndef MAXENGINE_CUBE_H
#define MAXENGINE_CUBE_H
#include "../GameObject.h"
#include "../../components/Transform.h"

class Cube : public GameObject {
public:
    static Cube* BuildCube();

    Transform* transform;

    void Start() override;

    void Update(float delta_time) override;
};


#endif //MAXENGINE_CUBE_H