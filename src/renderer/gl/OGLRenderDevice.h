//
// Created by zykov on 5/1/2026.
//

#ifndef MAXENGINE_RENDERDEVICE_H
#define MAXENGINE_RENDERDEVICE_H
#include <memory>

#include "OGLCommandPoolManager.h"
#include "OGLDescriptorSetManager.h"
#include "../IRenderDevice.h"
#include "../domain/Mesh.h"
#include "model/TextureData.h"
#include "../domain/struct/VertexLayout.h"
#include "GLFW/glfw3.h"
#include "managers/BufferManager.h"
#include "OGLShaderManager.h"
#include "OGLInstance.h"
#include "OGLTextureManager.h"
#include "OGLVertexBufferManager.h"
#include "../domain/IWindowContext.h"

class MeshManager;

namespace Rendering {
    struct OGLGraphicsShader {
        GLuint vShader;
        GLuint fShader;
        std::vector<OGLDescriptorSetLayout> layouts;
    };

    class OGLPipelineManager;

    class OGLRenderDevice : public IRenderDevice {
    public:
        explicit OGLRenderDevice(IWindow& window);

        ~OGLRenderDevice() override;

        CommandBuffer * AllocateCommandBuffer() override;

        TextureHandle CreateTexture(const TextureCreateRequest &desc, const void *initialData) override;

        ResourceSetLayoutHandle CreateResourceSetLayout(const ResourceSetLayoutDesc &resourceSetLayoutDesc) override;

        ResourceSetHandle CreateResourceSet(const PipelineHandle pipelineHandle, const ResourceBinding resource) override;

        ResourceSetHandle CreateResourceSet(const ResourceSetLayoutHandle resourceSetLayoutHandle) override;

        void UpdateResourceSet(ResourceSetHandle setHandle, const uint32_t binding, const TextureHandle textureHandle) override;

        void UpdateResourceSet(ResourceSetHandle setHandle, const uint32_t binding, const BufferHandle bufferHandle) override;

        BufferHandle CreateBuffer(const BufferDesc &desc, const void *data) override;

        BufferHandle CreateFrameBuffer(TextureHandle texture) override;

        void UpdateBuffer(BufferHandle id, const void *data, int offset, int size) override;

        void DestroyBuffer(BufferHandle id) override;

        void BindUniformBuffer(BufferHandle id, uint32_t index) override;

        PipelineHandle CreatePipeline(const std::vector<ShaderHandle> &shaders,
                                                         const RenderPassHandle renderPass,
                                                         const PipelineStateDesc &pipelineStateDesc) override;

        PipelineHandle CreatePipeline(const ShaderHandle shaderHandle, const PipelineStateDesc &pipelineStateDesc) override;

        PipelineHandle CreatePipeline(const ShaderDesc &shaderDesc, const PipelineStateDesc &pipelineStateDesc) override;

        void SubmitCommandBuffer(CommandBuffer *commandBuffer) override;

        void Present() override;

        ImageFormat GetDisplayFormat() override;



    private:
        OGLInstance* instance;
        OGLVertexBufferManager* bufferManager;
        OGLTextureManager* textureManager;
        OGLShaderManager* shaderManager;
        OGLPipelineManager* pipelineManager;
        OGLCommandPoolManager* commandPoolManager;
        OGLRenderPassManager* renderPassManager;
        OGLDescriptorSetManager* descriptorSetManager;

        IWindowContext* context = nullptr;
        GLFWwindow* window{};

        std::unordered_map<MeshId, GLuint> meshIds;

        std::vector<GLuint> shaders;
        std::vector<uint32_t> buffers;
        std::vector<TextureData> textures;

        GLuint cameraUbo{};
        GLuint lightUbo{};
        PipelineHandle currentPipeline{};

        CommandBuffer* commandBuffer{};

        std::unordered_map<TextureWrap, GLint> textureWrapMap = {
            {TextureWrap::Repeat, GL_REPEAT },
            {TextureWrap::ClampToEdge, GL_CLAMP_TO_EDGE },
            {TextureWrap::MirroredRepeat, GL_MIRRORED_REPEAT },
            {TextureWrap::ClampToBorder, GL_CLAMP_TO_BORDER }
        };

        std::vector<OGLGraphicsShader> _graphicsShaders;

        void SetupVertexLayoutForPSO(GLuint vao, const VertexLayout& layout);
        GLint MapGLComponents(VertexFormat format);

        GLenum MapTextureInternalFormat(TextureFormat format);
        GLenum MapTextureExternalFormat(TextureFormat format);

        GLenum MapCullMode(CullMode mode);
        GLenum MapFillMode(FillMode mode);

    };


} // namespace Rendering


#endif //MAXENGINE_RENDERDEVICE_H
