//
// Created by zykov on 4/24/2026.
//

#include "AmbientLight.h"


AmbientLight::AmbientLight() {
    handle = DIContainer::GetInstance().Get<IRenderer>()->AddLight(LightType::Ambient);
    intensity = 0.1f;
}

bool isReturned = false;

void AmbientLight::Update(const float delta_time) {
    Light::Update(delta_time);

    // if (intensity < 2.0f && !isReturned) {
    //     intensity += intensity * 2 * delta_time;
    // } else {
    //     isReturned = true;
    // }
    //
    // if (isReturned) {
    //     intensity -= intensity * 2 * delta_time;
    //
    //     if (intensity <= 0.5f) {
    //         isReturned = false;
    //     }
    // }
}
