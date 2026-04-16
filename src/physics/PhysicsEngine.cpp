//
// Created by zykov on 4/13/2026.
//

#include "../physics//PhysicsEngine.h"
#include <iostream>

#include "../components/Transform.h"
#include "../gameObject/GameObject.h"
#include "../components/physics/collision/BoxCollider.h"

void PhysicsEngine::Update() {
   CheckCollisions();
}

void PhysicsEngine::Register(Collider *collider) {
    colliders.push_back(collider);
}

void PhysicsEngine::Unregister(Collider *collider) {
    std::erase(colliders, collider);
}

void PhysicsEngine::CheckCollisions() {
    for (int i = 0; i < colliders.size() - 1; i++) {
        for (int j = i + 1; j < colliders.size(); j++) {

            const auto* collider1 = dynamic_cast<BoxCollider*>(colliders[i]);
            const auto* collider2 = dynamic_cast<BoxCollider*>(colliders[j]);

            if (collider1 == nullptr || collider2 == nullptr) {
                continue;
            }

            if (collider1->collision.Intersect(collider2->collision)) {
                ResolveCollisions(collider1, collider2);
            }
        }
    }
}

void PhysicsEngine::ResolveCollisions(const BoxCollider* bc1, const BoxCollider* bc2) {
    const AABB a = bc1->collision;
    const AABB b = bc2->collision;

    Vector3 overlap = AABB::CalculateOverlap(a, b);

    if (overlap.x < overlap.y && overlap.x < overlap.z) {
        const float dir = (a.min.x < b.min.x) ? -overlap.x : overlap.x;
        bc1->GetGameObject()->GetComponent<Transform>()->Translate(Vector3::Right() * dir);
    }
    else if (overlap.y < overlap.x && overlap.y < overlap.z) {
        const float dir = (a.min.y < b.min.y) ? -overlap.y : overlap.y;
        bc1->GetGameObject()->GetComponent<Transform>()->Translate(Vector3::Up() * dir);

        // if (dir > 0) player.velocity.y = 0;
    }
    else {
        const float dir = (a.min.z < b.min.z) ? -overlap.z : overlap.z;
        bc1->GetGameObject()->GetComponent<Transform>()->Translate(Vector3::Forward() * dir);
    }
}
