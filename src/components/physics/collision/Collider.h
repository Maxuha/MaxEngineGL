//
// Created by zykov on 4/14/2026.
//

#ifndef MAXENGINE_COLLIDER_H
#define MAXENGINE_COLLIDER_H
#include "../../Component.h"


class Collider : public Component {
public:
    explicit Collider(GameObject* gameObject);
};



#endif //MAXENGINE_COLLIDER_H
