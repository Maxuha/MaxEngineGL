//
// Created by zykov on 3/17/2026.
//

#include "Light.h"

Light::Light(){
}

Rendering::LightHandle Light::GetHandle() const {
    return handle;
}

void Light::Update(float delta_time) {
    Component::Update(delta_time);
}
