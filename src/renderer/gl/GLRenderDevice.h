//
// Created by zykov on 5/1/2026.
//

#ifndef MAXENGINE_RENDERDEVICE_H
#define MAXENGINE_RENDERDEVICE_H
#include <memory>
#include "../IRenderDevice.h"
#include "../../graphics/Shader.h"
#include "../domain/struct/PipelineHandle.h"
#include "model/TextureData.h"
#include "../domain/struct/VertexLayout.h"

class MeshManager;

namespace Rendering {
    class IWindowContext;

    class GLRenderDevice : public IRenderDevice {
    public:
        GLRenderDevice(IWindowContext& context);

        ~GLRenderDevice() override;

        ShaderId CreateShader(const char *vCode, const char *fCode) override;

        TextureId CreateTexture(const TextureCreateRequest &desc, const void *initialData) override;

        void UpdateTextureFilterMode(TextureId &id, TextureWrap textureWrap) override;

        IResourceSet* CreateResourceSet() override;

        BufferId CreateBuffer(const BufferDesc &desc, const void *data) override;

        void UpdateBuffer(BufferId id, void *data, int offset, int size) override;

        void DestroyBuffer(BufferId id) override;

        void Prepare() override;

        void DrawIndexed(size_t indexCount) override;

        void BindResourceSet(const ResourceSet &resourceSet) override;

        void BindPipeline(ShaderId shaderId) override;

        void BindMesh(MeshId meshId) override;

        void BindVertexBuffer(BufferId id) override;

        void BindIndexBuffer(BufferId id) override;

        void BindUniformBuffer(BufferId id, uint32_t index) override;

        void PushConstants(std::string key, Matrix4x4 mat) override;

        void PushConstants(const std::string &key, int value) override;

        void PushConstants(const std::string &key, float value) override;

        void PushConstants(const std::string &key, Color value) override;

        void PushConstants(const std::string &key, Vector3 value) override;

        void DrawIndexed(unsigned count, MeshId vao) override;

        void SetViewport(Viewport viewport) override;

        void SetCullMode(CullMode mode) override;

        void BeginRenderPass(ClearFlags flags, Color color, float depth) override;

        PipelineHandle CreatePipeline(const std::vector<ShaderInput> &shaders, const RenderState &renderState) override;

        PipelineHandle CreatePipelineState(const PipelineStateDesc& pipeline) override;

        void SetPipelineState(PipelineHandle pipeline) override;

        TextureData GetTextureData(TextureId id) override;

        void SubmitCommandBuffer(ICommandBuffer *commandBuffer) override;

        void Present() override;

    private:
        IWindowContext& context;
        GLFWwindow* window{};

        std::unordered_map<MaterialId, GLuint> materialIds;
        std::unordered_map<MeshId, GLuint> meshIds;

        std::vector<GLuint> shaders;
        std::vector<Pipeline> psoPool;
        std::vector<uint32_t> buffers;
        std::vector<TextureData> textures;
        std::vector<IPipeline*> pipelines;

        GLuint cameraUbo{};
        GLuint lightUbo{};
        GLuint currentPipeline{};
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

    };


} // namespace Rendering


#endif //MAXENGINE_RENDERDEVICE_H
