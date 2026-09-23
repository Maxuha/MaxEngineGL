//
// Created by zykov on 3/17/2026.
//

#ifndef MAXENGINE_LIGHT_H
#define MAXENGINE_LIGHT_H

#include "../../components/Component.h"
#include "../../math/Color.h"
#include "../../renderer/domain/LightRenderData.h"

class Light : public Component {
public:
    float intensity = 1.0f;
    Color color = Color(1, 1, 1, 1);

    Rendering::LightType lightType = Rendering::LightType::Ambient;

    Light();

    Rendering::LightHandle GetHandle() const;

    void Update(float delta_time) override;

protected:
    Rendering::LightHandle handle;

// private:
//     LightItemRenderData renderData;

};


#endif //MAXENGINE_LIGHT_H
