//
// Created by zykov on 3/17/2026.
//

#include "DirectionalLight.h"


DirectionalLight::DirectionalLight() {
    type = LightType::Directional;
    handle = DIContainer::GetInstance().Get<Rendering::IRenderer>()->AddLight(type);
}

void DirectionalLight::Update(const float delta_time) {
    Light::Update(delta_time);

    //GetGameObject()->GetTransform()->RotatePitch(180 * delta_time);
}
