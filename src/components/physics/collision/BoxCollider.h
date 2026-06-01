//
// Created by zykov on 4/11/2026.
//

#ifndef MAXENGINE_BOXCOLLIDER_H
#define MAXENGINE_BOXCOLLIDER_H
#include "Collider.h"
#include "../../../math/AABB.h"


class BoxCollider : public Collider {
public:
    explicit BoxCollider() : Collider() {}
    ~BoxCollider() = default;

    Vector3 size = Vector3(1, 1, 1);
    Vector3 offset = Vector3(0, 0, 0);

    AABB collision{};

    void Update(float delta_time) override;

private:
    void SyncAABB();
};



#endif //MAXENGINE_BOXCOLLIDER_H
