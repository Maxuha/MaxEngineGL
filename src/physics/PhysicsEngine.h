//
// Created by zykov on 4/13/2026.
//

#ifndef MAXENGINE_PHYSICSENGINE_H
#define MAXENGINE_PHYSICSENGINE_H

#include <vector>

class Collider;
class BoxCollider;

class PhysicsEngine {
public:
    void Update();

    void Register(Collider* collider);

    void Unregister(Collider* collider);

private:
    std::vector<Collider*> colliders;

    void CheckCollisions();
    void ResolveCollisions(const BoxCollider* bc1, const BoxCollider* bc2);
};


#endif //MAXENGINE_PHYSICSENGINE_H
