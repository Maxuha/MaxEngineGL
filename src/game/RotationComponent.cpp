//
// Created by zykov on 7/30/2026.
//

#include "RotationComponent.h"

#include "../gameObject/GameObject.h"

namespace Game {
    void RotationComponent::Start() {
        Behaviour::Start();

    }

    void RotationComponent::Update(const float delta_time) {
        Behaviour::Update(delta_time);

        GetGameObject()->GetComponent<Transform>()->RotateRoll(delta_time * 360);
    }

    RotationComponent::~RotationComponent() {
    }
} // Game