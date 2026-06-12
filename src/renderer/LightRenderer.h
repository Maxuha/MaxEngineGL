//
// Created by zykov on 5/6/2026.
//

#ifndef MAXENGINE_LIGHTHANDLER_H
#define MAXENGINE_LIGHTHANDLER_H
#include "AmbientLightData.h"
#include "ILightRenderer.h"
#include "IRenderDevice.h"

class SpotLight;
class PointLight;
class DirectionalLight;
class AmbientLight;
class IRenderDevice;

namespace Rendering {

class LightRenderer: public ILightRenderer {
public:
    explicit LightRenderer(IRenderDevice& renderDevice);
    ~LightRenderer() override;

    LightData GetLightData() const;

    void LoadLights(Scene& scene) override;
    void EnableLights() override;
    void Clear() override;

private:
    LightData lightData{};
    IRenderDevice* renderDevice;

    BufferHandle bufferId;

    int indexPointLight = 0;
    int maxPointLights = 8;

    int indexSpotLight = 0;
    int maxSpotLights = 8;

    void SetLight(const AmbientLightData &light);
    void SetLight(const DirectionalLightData &light);
    void SetLight(const PointLightData &light);
    void SetLight(const SpotLightData &light);

    void ExtractLight(const AmbientLight& light);
    void ExtractLight(const DirectionalLight& light);
    void ExtractLight(const PointLight& light);
    void ExtractLight(const SpotLight& light);
};

} // namespace Rendering


#endif //MAXENGINE_LIGHTHANDLER_H
