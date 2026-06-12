//
// Created by zykov on 5/1/2026.
//

#ifndef MAXENGINE_RENDERDEVICE_H
#define MAXENGINE_RENDERDEVICE_H
#include <memory>
#include "../IRenderDevice.h"
#include "../domain/Shader.h"
#include "../domain/struct/PipelineHandle.h"
#include "model/TextureData.h"
#include "../domain/struct/VertexLayout.h"
#include "managers/BufferManager.h"
#include "model/PipelineData.h"

class MeshManager;

namespace Rendering {
    class IWindowContext;

    class GLRenderDevice : public IRenderDevice {
    public:
        explicit GLRenderDevice(IWindowContext& context);

        ~GLRenderDevice() override;

        ShaderId CreateShader(const char *vCode, const char *fCode) override;

        TextureHandle CreateTexture(const TextureCreateRequest &desc, const void *initialData) override;

        void UpdateTextureFilterMode(TextureHandle &id, TextureWrap textureWrap) override;

        IResourceSet* CreateResourceSet() override;

        BufferHandle CreateBuffer(const BufferDesc &desc, const void *data) override;

        BufferHandle CreateFrameBuffer(TextureHandle texture) override;

        void UpdateBuffer(BufferHandle id, void *data, int offset, int size) override;

        void DestroyBuffer(BufferHandle id) override;

        void BindUniformBuffer(BufferHandle id, uint32_t index) override;

        PipelineHandle CreatePipeline(const std::vector<ShaderInput> &shaders, const PipelineStateDesc &pipelineStateDesc) override;

        void SubmitCommandBuffer(ICommandBuffer *commandBuffer) override;

        void Present() override;

    private:
        BufferManager* bufferManager;

        IWindowContext& context;
        GLFWwindow* window{};

        std::unordered_map<MaterialHandle, GLuint> materialIds;
        std::unordered_map<MeshId, GLuint> meshIds;

        std::vector<GLuint> shaders;
        std::vector<Pipeline> psoPool;
        std::vector<uint32_t> buffers;
        std::vector<TextureData> textures;
        std::vector<PipelineData> pipelines;

        GLuint cameraUbo{};
        GLuint lightUbo{};
        PipelineHandle currentPipeline{};
        Pipeline* currentPso = nullptr;

        ICommandBuffer* commandBuffer{};

        std::unordered_map<TextureWrap, GLint> textureWrapMap = {
            {TextureWrap::Repeat, GL_REPEAT },
            {TextureWrap::ClampToEdge, GL_CLAMP_TO_EDGE },
            {TextureWrap::MirroredRepeat, GL_MIRRORED_REPEAT },
            {TextureWrap::ClampToBorder, GL_CLAMP_TO_BORDER }
        };

        void SetupVertexLayoutForPSO(GLuint vao, const VertexLayout& layout);
        GLint MapGLComponents(VertexFormat format);

        GLenum MapTextureInternalFormat(TextureFormat format);
        GLenum MapTextureExternalFormat(TextureFormat format);

        GLenum MapCullMode(CullMode mode);
        GLenum MapFillMode(FillMode mode);

    };


} // namespace Rendering


#endif //MAXENGINE_RENDERDEVICE_H
