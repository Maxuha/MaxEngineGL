//
// Created by zykov on 5/6/2026.
//

#include "LightRenderer.h"
#include "AmbientLightData.h"
#include "../Scene.h"
#include "../components/light/Light.h"
#include "../components/light/AmbientLight.h"
#include "../components/light/DirectionalLight.h"
#include "../components/light/PointLight.h"
#include "../components/light/SpotLight.h"
#include "domain/buffer/BufferDesc.h"

namespace Rendering {
    LightRenderer::LightRenderer(IRenderDevice &renderDevice) : bufferId() {
        this->renderDevice = &renderDevice;
        maxPointLights = std::size(lightData.pointLights);
        maxSpotLights = std::size(lightData.spotLights);

        constexpr auto bufferDesc = BufferDesc{
            .size = sizeof(LightData),
            .index = 1,
            .usage = BufferUsage::Uniform
        };
        bufferId = renderDevice.CreateBuffer(bufferDesc, &lightData);
        // TO DO fix bufferId
        renderDevice.BindUniformBuffer(bufferId, 1);
    }

    LightRenderer::~LightRenderer() = default;

    LightData LightRenderer::GetLightData() const {
        return lightData;
    }

    void LightRenderer::LoadLights(Scene &scene) {
        std::vector<Light *> lights = scene.GetComponents<Light>();
        for (const auto &light: lights) {
            if (light == nullptr) continue;

            if (auto l = dynamic_cast<AmbientLight *>(light)) ExtractLight(*l);
            else if (auto l = dynamic_cast<DirectionalLight *>(light)) ExtractLight(*l);
            else if (auto l = dynamic_cast<PointLight *>(light)) ExtractLight(*l);
            else if (auto l = dynamic_cast<SpotLight *>(light)) ExtractLight(*l);
        }
    }

    void LightRenderer::EnableLights() {
        renderDevice->UpdateBuffer(bufferId, &lightData, 0, sizeof(LightData));
        // renderDevice->SetLight(lightData);
    }

    void LightRenderer::Clear() {
        lightData = LightData{};
        indexPointLight = 0;
        indexSpotLight = 0;
    }

    void LightRenderer::SetLight(const AmbientLightData &light) {
        lightData.ambientLight = light;
    }

    void LightRenderer::SetLight(const DirectionalLightData &light) {
        lightData.directionalLight = light;
    }

    void LightRenderer::SetLight(const PointLightData &light) {
        lightData.pointLights[indexPointLight] = light;
        indexPointLight++;
    }

    void LightRenderer::SetLight(const SpotLightData &light) {
        lightData.spotLights[indexSpotLight] = light;
        indexSpotLight++;
    }

    void LightRenderer::ExtractLight(const AmbientLight &light) {
        SetLight(AmbientLightData{.colorIntensity = Vector4(light.color.rgb(), light.intensity)});
    }

    void LightRenderer::ExtractLight(const DirectionalLight &light) {
        SetLight(DirectionalLightData{
            // to fix
            // .projection = light.GetGameObject()->GetTransform()->Othographic(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 200.0f),
            // .view = light.GetGameObject()->GetTransform()->LookAt(),
            .space = light.GetGameObject()->GetTransform()->Othographic(-20.0f, 20.0f, -20.0f, 20.0f, 0.01f, 200.0f) *
                     light.GetGameObject()->GetTransform()->LookAt(),
            .colorIntensity = Vector4(light.color.rgb(), light.intensity),
            .direction = Vector4(light.GetGameObject()->GetTransform()->Forward(), 0.0f)
        });
    }

    void LightRenderer::ExtractLight(const PointLight &light) {
        SetLight(
            PointLightData{
                .colorIntensity = Vector4(light.color.rgb(), light.intensity),
                .positionRange = Vector4(light.GetGameObject()->GetTransform()->position, light.GetRange())
            }
        );
    }

    void LightRenderer::ExtractLight(const SpotLight &light) {
        SetLight(
            SpotLightData{
                .colorIntensity = Vector4(light.color.rgb(), light.intensity),
                .positionRange = Vector4(light.GetGameObject()->GetTransform()->position, light.GetRange()),
                .direction = Vector4(light.GetGameObject()->GetTransform()->Forward(), 0.0f),
                .coneAngle = Vector4(light.GetInnerAngle(), light.GetOuterAngle(), 0.0f, 0.0f)
            });
    }
} // namespace Rendering
