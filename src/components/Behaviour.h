//
// Created by zykov on 7/30/2026.
//

#ifndef MAXENGINE_BEHAVIOUR_H
#define MAXENGINE_BEHAVIOUR_H
#include "Component.h"

class Behaviour : public Component {
public:
    void Start() override;
    void Update(float delta_time) override;
    void Destroy();
};

#endif //MAXENGINE_BEHAVIOUR_H
