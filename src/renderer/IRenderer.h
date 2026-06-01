//
// Created by zykov on 4/30/2026.
//

#ifndef MAXENGINE_IRENDERER_H
#define MAXENGINE_IRENDERER_H

#include "../Scene.h"
#include "../graphics/Mesh.h"


class Camera;

namespace Rendering {
    class IRenderer {
    public:
        virtual ~IRenderer() = default;

        virtual PipelineHandle CreatePipeline(const std::vector<ShaderInput> &shaders, const RenderState &renderState) = 0;

        virtual TextureId CreateTexture(void *data, TextureCreateRequest& textureCreateDesc) = 0;

        virtual void UpdateTextureFilterMode(TextureId& textureId, TextureWrap textureWrap) = 0;

        virtual Mesh *CreateMesh(std::span<const Vertex> vertices, std::span<const uint32_t> indices) = 0;

        virtual void Submit(Scene &scene) = 0;

        virtual void BeginFrame(Camera &camera, Shader *shader) = 0;

        virtual void Render() = 0;

        virtual void EndFrame() = 0;
    };
} // namespace Rendering


#endif //MAXENGINE_IRENDERER_H
