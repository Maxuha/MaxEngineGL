//
// Created by zykov on 4/11/2026.
//

#include "BoxCollider.h"
#include "../../Transform.h"
#include "../../../gameObject/GameObject.h"


void BoxCollider::Update(float delta_time) {
    Component::Update(delta_time);
    SyncAABB();
}

void BoxCollider::SyncAABB() {
    const auto transform = GetGameObject()->GetComponent<Transform>();
    const auto worldMatrix = transform->GetWorldMatrix();

    collision.min = worldMatrix * (offset - size);
    collision.max = worldMatrix * (offset + size);
};
