//
// Created by zykov on 6/19/2026.
//

#ifndef MAXENGINE_VULKANPIPELINE_H
#define MAXENGINE_VULKANPIPELINE_H
#include "IVulkanPipelineManager.h"
#include "../../IPipelineManager.h"
#include "../shaders/VulkanShaderManager.h"
#include "../src/renderer/RenderContext.h"

namespace Rendering {
    struct VulkanPipeline {
        VkPipeline Pipeline;
        std::vector<VkDescriptorSetLayout> descriptionSetLayouts;
        VkPipelineLayout pipelineLayout;
    };

    class VulkanPipelineManager : public IPipelineManager {
    public:
        explicit VulkanPipelineManager(VulkanLogicalDevice& device, VulkanShaderManager& shaderManager);

        ~VulkanPipelineManager() override;

        PipelineHandle CreatePipeline(const std::vector<ShaderHandle> &shaders, RenderPassHandle renderPass, const PipelineStateDesc &pipelineStateDesc) override;

        PipelineHandle CreatePipeline(const GraphicsVulkanShader& shader, const PipelineStateDesc &pipelineStateDesc);

        VulkanPipeline GetPipeline(PipelineHandle handle) const;

        VkDescriptorSetLayout CreateDescriptorSetLayout(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags) const;

        VkDescriptorSetLayout CreateDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings) const;

    private:
        VulkanLogicalDevice& logicalDevice;
        VulkanShaderManager& shaderManager;

        std::vector<VulkanPipeline> pipelines;
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts;

        VkDescriptorPool CreateDescriptorPool(size_t size) const;

        VkPipelineDynamicStateCreateInfo CreateDynamicState(const DynamicStateDesc& dynamicState);

        VkFormat MapVertexFormat(VertexFormat format);

        VkPipelineVertexInputStateCreateInfo CreateVertexInputState(const VertexLayout& layout);

        VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyState();

        VkPipelineRasterizationStateCreateInfo CreateRasterizationState(const RasterizerDesc& rasterizerDesc);

        VkPipelineMultisampleStateCreateInfo CreateMultisampleState();

        VkPipelineColorBlendStateCreateInfo CreateColorBlendAttachmentState(const BlendAttachmentDesc& attachments);

        VkPipelineColorBlendStateCreateInfo CreateColorBlendState();

        VkPipelineDepthStencilStateCreateInfo CreateDepthStencilState();

        VkPipelineViewportStateCreateInfo CreateViewportState(const Rect& rect, VkViewport& viewport, VkRect2D& scissor);

        VkPipelineLayout CreatePipelineLayout(const std::vector<VkDescriptorSetLayout> &descriptorSetLayouts = {}) const;

        std::vector<VkPipelineShaderStageCreateInfo> CreateShaderStages(const std::vector<ShaderHandle> &shaderHandles) const;

        std::vector<VkPipelineShaderStageCreateInfo> CreateShaderStages(const GraphicsVulkanShader& shader) const;

        VkPipeline CreateGraphicsPipeline(
            const VkPipelineShaderStageCreateInfo *shaderStages,
            uint32_t shaderStageCount,
            const VkPipelineVertexInputStateCreateInfo& vertexInputInfo,
            const VkPipelineInputAssemblyStateCreateInfo& inputAssembly,
            const VkPipelineViewportStateCreateInfo& viewportState,
            const VkPipelineRasterizationStateCreateInfo& rasterizer,
            const VkPipelineMultisampleStateCreateInfo& multisampling,
            const VkPipelineColorBlendStateCreateInfo& colorBlending,
            const VkPipelineDynamicStateCreateInfo& dynamicState,
            VkPipelineLayout pipelineLayout,
            VkRenderPass renderPass
        ) const;

        VkPipelineColorBlendAttachmentState MapBlendMode(BlendMode mode);
        VkDynamicState MapDynamicState(DynamicState state);

        VkFormat MapImageFormat(ImageFormat imageFormat);
    };
} // Rendering

#endif //MAXENGINE_VULKANPIPELINE_H
