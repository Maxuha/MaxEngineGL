//
// Created by zykov on 3/15/2026.
//

#ifndef MAXENGINE_CUBE_H
#define MAXENGINE_CUBE_H
#include "../GameObject.h"
#include "../../components/Camera.h"


class Cube : public GameObject{
public:
    static Cube* BuildCube(Camera* camera);
};


#endif //MAXENGINE_CUBE_H