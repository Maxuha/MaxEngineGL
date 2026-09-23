//
// Created by zykov on 3/17/2026.
//

#include "DirectionalLight.h"


DirectionalLight::DirectionalLight() {
    lightType = Rendering::LightType::Directional;
    handle = DIContainer::GetInstance().Get<Rendering::IRenderer>()->CreateLight(lightType);
}

void DirectionalLight::Update(const float delta_time) {
    Light::Update(delta_time);

    //GetGameObject()->GetTransform()->RotatePitch(180 * delta_time);
}
