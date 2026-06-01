//
// Created by zykov on 5/7/2026.
//

#ifndef MAXENGINE_ILIGHTRENDERER_H
#define MAXENGINE_ILIGHTRENDERER_H


class Scene;

namespace Rendering {

class ILightRenderer {
public:
    virtual ~ILightRenderer() = default;

    virtual void LoadLights(Scene& scene) = 0;
    virtual void EnableLights() = 0;
    virtual void Clear() = 0;
};

} // namespace Rendering


#endif //MAXENGINE_ILIGHTRENDERER_H
