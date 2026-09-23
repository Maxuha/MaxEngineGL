//
// Created by zykov on 4/30/2026.
//

#ifndef MAXENGINE_IRENDERER_H
#define MAXENGINE_IRENDERER_H

#include "../Scene.h"
#include "domain/CameraRender.h"
#include "domain/LightRenderData.h"
#include "domain/Mesh.h"
#include "domain/Texture.h"
#include "domain/struct/PipelineStateDesc.h"
#include "gl/desc/TextureCreateRequest.h"

class MaterialHandle;

namespace Rendering {

    struct CameraRenderData {
        BufferHandle ViewProjectionBuffer;
        ResourceSetHandle ResourceSet;
    };

    struct CreateLightDesc {
        LightType type;
        float intensity{};
        Color color;
        Vector3 position;
        Vector3 direction;
        float innerConeAngle{};
        float outerConeAngle{};
    };

    class IRenderer {
    public:
        virtual ~IRenderer() = default;

        virtual CameraRender* CreateCamera(float fov, float zNear, float zFar, float aspectRatio) = 0;

        virtual Mesh *CreateMesh(const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices) = 0;

        virtual TextureHandle CreateTexture(void *data, TextureCreateRequest& textureCreateDesc) = 0;

        virtual MaterialHandle CreateMaterial(const ShaderHandle shader, const MaterialDesc &materialDesc) = 0;

        virtual ShaderHandle CreateShader(const ShaderDesc& shaderDesc) = 0;

        virtual LightHandle CreateLight(LightType type) = 0;

        //material

        virtual void BindTextureToMaterial(const Material& material, uint32_t slot, const Texture& texture) = 0;

        virtual void BindBufferToMaterial(MaterialHandle materialHandle, uint32_t slot, BufferHandle bufferId) = 0;

        // rendering

        virtual void BeginFrame(Camera &camera) = 0;

        virtual void Render(Scene &scene) = 0;

        virtual void EndFrame() = 0;

        virtual void SetDepthShader(Shader& shader) = 0;
    };
} // namespace Rendering


#endif //MAXENGINE_IRENDERER_H
