//
// Created by zykov on 3/22/2026.
//

#ifndef MAXENGINE_RENDERCONTEXT_H
#define MAXENGINE_RENDERCONTEXT_H
#include "../math/Matrix4x4.h"
#include "../math/Vector3.h"

struct RenderContext {
    Matrix4x4 viewMatrix{0};
    Matrix4x4 projectionMatrix{0};

    Vector3 lightColor{};
    Vector3 lightDirection{};
    float lightIntensity{};
};


#endif //MAXENGINE_RENDERCONTEXT_H