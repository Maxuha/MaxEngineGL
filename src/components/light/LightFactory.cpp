//
// Created by zykov on 5/5/2026.
//

#include "LightFactory.h"

#include "AmbientLight.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "../../gameObject/GameObject.h"

GameObject * LightFactory::SpawnAmbientLight() {
    auto* go = new GameObject();
    go->AddComponent<AmbientLight>();
    return go;
}

GameObject * LightFactory::SpawnDirectionalLight() {
    auto* go = new GameObject();
    go->AddComponent<DirectionalLight>();
    return go;
}

GameObject * LightFactory::SpawnPointLight() {
    auto* go = new GameObject();
    go->AddComponent<PointLight>();
    return go;
}

GameObject * LightFactory::SpawnSpotLight() {
    auto* go = new GameObject();
    go->AddComponent<SpotLight>();
    return go;
}
