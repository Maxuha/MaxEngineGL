//
// Created by zykov on 7/30/2026.
//

#ifndef MAXENGINE_ROTATIONCOMPONENT_H
#define MAXENGINE_ROTATIONCOMPONENT_H
#include "../components/Behaviour.h"

namespace Game {
    class RotationComponent : public Behaviour {
    public:
        void Start() override;

        void Update(float delta_time) override;

        ~RotationComponent() override;
    };
} // Game

#endif //MAXENGINE_ROTATIONCOMPONENT_H
