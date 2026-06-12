//
// Created by zykov on 5/5/2026.
//

#ifndef MAXENGINE_LIGHTFACTORY_H
#define MAXENGINE_LIGHTFACTORY_H
#include "Light.h"


class LightFactory {
public:
    GameObject *SpawnAmbientLight();

    GameObject *SpawnDirectionalLight();

    GameObject *SpawnPointLight();

    GameObject *SpawnSpotLight();
};


#endif //MAXENGINE_LIGHTFACTORY_H
