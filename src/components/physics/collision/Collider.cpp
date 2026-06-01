//
// Created by zykov on 4/14/2026.
//

#include "Collider.h"
#include "../../../di/DIContainer.h"

Collider::Collider(): Component() {
    DIContainer::GetInstance().Get<PhysicsEngine>()->Register(this);
}
