//
// Created by zykov on 6/19/2026.
//

#include "VulkanPipelineManager.h"


namespace Rendering {
    VulkanPipelineManager::VulkanPipelineManager(VulkanLogicalDevice &device,
                                                 VulkanShaderManager &shaderManager) : logicalDevice(device),
        shaderManager(shaderManager) {
    }

    VulkanPipelineManager::~VulkanPipelineManager() {
        for (const auto pipeline: pipelines) {
            vkDestroyPipeline(logicalDevice.GetDevice(), pipeline.Pipeline, nullptr);
        }
    }

    PipelineHandle VulkanPipelineManager::CreatePipeline(const std::vector<ShaderHandle> &shaders,
                                                         const RenderPassHandle renderPass,
                                                         const PipelineStateDesc &pipelineStateDesc) {
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages = CreateShaderStages(shaders);

        const VkPipelineDynamicStateCreateInfo dynamicState = CreateDynamicState(pipelineStateDesc.DynamicState);

        std::vector<VkVertexInputBindingDescription> bindingDescriptions;
        bindingDescriptions.resize(pipelineStateDesc.VertexLayout.bindings.size());

        for (uint32_t i = 0; i < pipelineStateDesc.VertexLayout.bindings.size(); i++) {
            const auto &binding = pipelineStateDesc.VertexLayout.bindings[i];
            bindingDescriptions[i].binding = binding.bufferIndex;
            bindingDescriptions[i].stride = binding.stride;
            //TODO Add input rate mapping
            bindingDescriptions[i].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        }

        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        attributeDescriptions.resize(pipelineStateDesc.VertexLayout.attributes.size());

        for (uint32_t i = 0; i < pipelineStateDesc.VertexLayout.attributes.size(); i++) {
            const auto &attribute = pipelineStateDesc.VertexLayout.attributes[i];
            attributeDescriptions[i].binding = attribute.bufferIndex;
            attributeDescriptions[i].location = attribute.location;
            attributeDescriptions[i].format = MapVertexFormat(attribute.format);
            attributeDescriptions[i].offset = attribute.offset;
        }

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = bindingDescriptions.size();
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data(); // Optional
        vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data(); // Optional

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        static VkViewport viewport{};
        static VkRect2D scissor{};

        const VkPipelineViewportStateCreateInfo viewportState = CreateViewportState(
            pipelineStateDesc.Viewport, viewport, scissor);

        const VkPipelineRasterizationStateCreateInfo rasterizer = CreateRasterizationState(
            pipelineStateDesc.RasterizerState);

        const VkPipelineMultisampleStateCreateInfo multisampling = CreateMultisampleState();

        VkPipelineColorBlendStateCreateInfo colorBlending = CreateColorBlendAttachmentState(
            pipelineStateDesc.BlendAttachmentDesc);

        const VkPipelineDepthStencilStateCreateInfo depthStencil = CreateDepthStencilState();

        VkDescriptorSetLayout descriptorSetLayout1 = CreateDescriptorSetLayout(
            0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
        VkDescriptorSetLayout descriptorSetLayout2 = CreateDescriptorSetLayout(
            1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
        VkDescriptorSetLayout descriptorSetLayout3 = CreateDescriptorSetLayout(
            2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

        std::vector descriptorSetLayouts = {
            descriptorSetLayout1, descriptorSetLayout2, descriptorSetLayout3,
        };

        VkPipelineLayout pipelineLayout = CreatePipelineLayout(descriptorSetLayouts);

        static VkFormat colorFormat = MapImageFormat(pipelineStateDesc.ColorFormat);
        static VkFormat depthFormat = MapImageFormat(pipelineStateDesc.DepthFormat);

        VkPipelineRenderingCreateInfo renderingCreateInfo{};
        renderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        renderingCreateInfo.pNext = nullptr;
        renderingCreateInfo.colorAttachmentCount = 1;
        renderingCreateInfo.pColorAttachmentFormats = &colorFormat;
        renderingCreateInfo.depthAttachmentFormat = depthFormat;
        renderingCreateInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext = &renderingCreateInfo;
        pipelineInfo.stageCount = shaderStages.size();
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = nullptr; // Optional
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = VK_NULL_HANDLE;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipelineInfo.basePipelineIndex = -1; // Optional

        VkPipeline pipeline;

        if (VkResult result = vkCreateGraphicsPipelines(logicalDevice.GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo,
                                                        nullptr,
                                                        &pipeline); result != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline! Error code: " + std::to_string(result));
        }

        const PipelineHandle pipelineHandle{.Id = pipelines.size()};

        VulkanPipeline pipelineData;
        pipelineData.Pipeline = pipeline;
        pipelineData.pipelineLayout = pipelineLayout;
        pipelineData.descriptionSetLayouts = descriptorSetLayouts;

        pipelines.push_back(pipelineData);

        return pipelineHandle;
    }

    PipelineHandle VulkanPipelineManager::CreatePipeline(const GraphicsVulkanShader &shader,
                                                         const PipelineStateDesc &pipelineStateDesc) {

        std::vector<VkPipelineShaderStageCreateInfo> shaderStages = CreateShaderStages(shader);

        const VkPipelineDynamicStateCreateInfo dynamicState = CreateDynamicState(pipelineStateDesc.DynamicState);

        std::vector<VkVertexInputBindingDescription> bindingDescriptions;
        bindingDescriptions.resize(pipelineStateDesc.VertexLayout.bindings.size());

        for (uint32_t i = 0; i < pipelineStateDesc.VertexLayout.bindings.size(); i++) {
            const auto &binding = pipelineStateDesc.VertexLayout.bindings[i];
            bindingDescriptions[i].binding = binding.bufferIndex;
            bindingDescriptions[i].stride = binding.stride;
            //TODO Add input rate mapping
            bindingDescriptions[i].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        }

        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        attributeDescriptions.resize(pipelineStateDesc.VertexLayout.attributes.size());

        for (uint32_t i = 0; i < pipelineStateDesc.VertexLayout.attributes.size(); i++) {
            const auto &attribute = pipelineStateDesc.VertexLayout.attributes[i];
            attributeDescriptions[i].binding = attribute.bufferIndex;
            attributeDescriptions[i].location = attribute.location;
            attributeDescriptions[i].format = MapVertexFormat(attribute.format);
            attributeDescriptions[i].offset = attribute.offset;
        }

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = bindingDescriptions.size();
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data(); // Optional
        vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data(); // Optional

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        static VkViewport viewport{};
        static VkRect2D scissor{};

        const VkPipelineViewportStateCreateInfo viewportState = CreateViewportState(
            pipelineStateDesc.Viewport, viewport, scissor);

        const VkPipelineRasterizationStateCreateInfo rasterizer = CreateRasterizationState(
            pipelineStateDesc.RasterizerState);

        const VkPipelineMultisampleStateCreateInfo multisampling = CreateMultisampleState();

        VkPipelineColorBlendStateCreateInfo colorBlending = CreateColorBlendAttachmentState(
            pipelineStateDesc.BlendAttachmentDesc);

        const VkPipelineDepthStencilStateCreateInfo depthStencil = CreateDepthStencilState();

        VkPipelineLayout pipelineLayout = CreatePipelineLayout(shader.DescriptorSetLayouts);

        static VkFormat colorFormat = MapImageFormat(pipelineStateDesc.ColorFormat);
        static VkFormat depthFormat = MapImageFormat(pipelineStateDesc.DepthFormat);

        VkPipelineRenderingCreateInfo renderingCreateInfo{};
        renderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
        renderingCreateInfo.pNext = nullptr;
        renderingCreateInfo.colorAttachmentCount = 1;
        renderingCreateInfo.pColorAttachmentFormats = &colorFormat;
        renderingCreateInfo.depthAttachmentFormat = depthFormat;
        renderingCreateInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext = &renderingCreateInfo;
        pipelineInfo.stageCount = shaderStages.size();
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = nullptr; // Optional
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = VK_NULL_HANDLE;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipelineInfo.basePipelineIndex = -1; // Optional

        VkPipeline pipeline;

        if (VkResult result = vkCreateGraphicsPipelines(logicalDevice.GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo,
                                                        nullptr,
                                                        &pipeline); result != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline! Error code: " + std::to_string(result));
        }

        const PipelineHandle pipelineHandle{.Id = pipelines.size()};

        VulkanPipeline pipelineData;
        pipelineData.Pipeline = pipeline;
        pipelineData.pipelineLayout = pipelineLayout;
        pipelineData.descriptionSetLayouts = shader.DescriptorSetLayouts;

        pipelines.push_back(pipelineData);

        return pipelineHandle;
    }

    VulkanPipeline VulkanPipelineManager::GetPipeline(const PipelineHandle handle) const {
        return pipelines[handle.Id];
    }

    VkDescriptorPool VulkanPipelineManager::CreateDescriptorPool(const size_t size) const {
        VkDescriptorPoolSize poolSize1{};
        poolSize1.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize1.descriptorCount = static_cast<uint32_t>(size);

        VkDescriptorPoolSize poolSize2{};
        poolSize2.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSize2.descriptorCount = static_cast<uint32_t>(size);

        const std::array poolSizes{poolSize1};

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = poolSizes.size();
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(size);

        VkDescriptorPool descriptorPool;

        if (vkCreateDescriptorPool(logicalDevice.GetDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }

        return descriptorPool;
    }

    VkDescriptorSetLayout VulkanPipelineManager::CreateDescriptorSetLayout(
        const uint32_t binding, const VkDescriptorType descriptorType, const VkShaderStageFlags stageFlags) const {
        VkDescriptorSetLayoutBinding layout_binding{};
        layout_binding.binding = binding;
        layout_binding.descriptorType = descriptorType;
        layout_binding.descriptorCount = 1;
        layout_binding.stageFlags = stageFlags;
        layout_binding.pImmutableSamplers = nullptr; // Optional

        const std::array bindings = {layout_binding};

        VkDescriptorSetLayout descriptorSetLayout;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());;
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(logicalDevice.GetDevice(), &layoutInfo, nullptr, &descriptorSetLayout) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
        return descriptorSetLayout;
    }

    VkDescriptorSetLayout VulkanPipelineManager::CreateDescriptorSetLayout(
        const std::vector<VkDescriptorSetLayoutBinding> &bindings) const {
        VkDescriptorSetLayout descriptorSetLayout;
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = bindings.size();
        layoutInfo.pBindings = bindings.data();
        if (vkCreateDescriptorSetLayout(logicalDevice.GetDevice(), &layoutInfo, nullptr, &descriptorSetLayout) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
        return descriptorSetLayout;
    }

    VkPipelineDynamicStateCreateInfo VulkanPipelineManager::CreateDynamicState(const DynamicStateDesc &dynamicState) {
        std::vector<VkDynamicState> dynamicStates(dynamicState.state.size());
        uint32_t activeStateCount = 0;

        for (const auto &state: dynamicState.state) {
            dynamicStates[activeStateCount++] = MapDynamicState(state);
        }

        VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
        dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateCreateInfo.dynamicStateCount = activeStateCount;
        dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();
        return dynamicStateCreateInfo;
    }

    VkFormat VulkanPipelineManager::MapVertexFormat(const VertexFormat format) {
        switch (format) {
            case VertexFormat::Float2:
                return VK_FORMAT_R32G32_SFLOAT;
                break;
            case VertexFormat::Float3:
                return VK_FORMAT_R32G32B32_SFLOAT;
                break;
            case VertexFormat::Float4:
                return VK_FORMAT_R32G32B32A32_SFLOAT;
                break;
            default:
                return VK_FORMAT_UNDEFINED;
        }
    }

    VkPipelineVertexInputStateCreateInfo VulkanPipelineManager::CreateVertexInputState(const VertexLayout &layout) {
        std::vector<VkVertexInputBindingDescription> bindingDescriptionsVec;
        bindingDescriptionsVec.resize(layout.bindings.size());

        for (uint32_t i = 0; i < layout.bindings.size(); i++) {
            const auto &binding = layout.bindings[i];
            bindingDescriptionsVec[i].binding = binding.bufferIndex;
            bindingDescriptionsVec[i].stride = binding.stride;
            //TODO Add input rate mapping
            bindingDescriptionsVec[i].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        }

        std::vector<VkVertexInputAttributeDescription> attributeDescriptionsVec;
        attributeDescriptionsVec.resize(layout.attributes.size());

        for (uint32_t i = 0; i < layout.attributes.size(); i++) {
            const auto &attribute = layout.attributes[i];
            attributeDescriptionsVec[i].binding = attribute.bufferIndex;
            attributeDescriptionsVec[i].location = attribute.location;
            attributeDescriptionsVec[i].format = MapVertexFormat(attribute.format);
            attributeDescriptionsVec[i].offset = attribute.offset;
        }

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = bindingDescriptionsVec.size();
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptionsVec.data(); // Optional
        vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptionsVec.size();
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptionsVec.data(); // Optional
        return vertexInputInfo;
    }

    VkPipelineInputAssemblyStateCreateInfo VulkanPipelineManager::CreateInputAssemblyState() {
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;
        return inputAssembly;
    }

    VkPipelineMultisampleStateCreateInfo VulkanPipelineManager::CreateMultisampleState() {
        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f; // Optional
        multisampling.pSampleMask = nullptr; // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable = VK_FALSE; // Optional
        return multisampling;
    }

    VkPipelineRasterizationStateCreateInfo VulkanPipelineManager::CreateRasterizationState(
        const RasterizerDesc &rasterizerDesc) {
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = rasterizerDesc.depthClampEnable ? VK_TRUE : VK_FALSE;
        rasterizer.rasterizerDiscardEnable = rasterizerDesc.rasterizerDiscardEnable ? VK_TRUE : VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = rasterizerDesc.lineWidth;
        rasterizer.cullMode = VK_CULL_MODE_NONE;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = rasterizerDesc.depthBiasEnable ? VK_TRUE : VK_FALSE;
        rasterizer.depthBiasConstantFactor = rasterizerDesc.depthBiasConstantFactor;
        rasterizer.depthBiasClamp = rasterizerDesc.depthBiasClamp;
        rasterizer.depthBiasSlopeFactor = rasterizerDesc.depthBiasClamp;
        return rasterizer;
    }

    VkPipelineColorBlendStateCreateInfo VulkanPipelineManager::CreateColorBlendAttachmentState(
        const BlendAttachmentDesc &attachments) {
        static std::vector<VkPipelineColorBlendAttachmentState> vkBlendAttachments;
        vkBlendAttachments.clear();
        vkBlendAttachments.reserve(attachments.blendMode.size());

        for (const auto blendMode: attachments.blendMode) {
            VkPipelineColorBlendAttachmentState colorBlendAttachment = MapBlendMode(blendMode);
            vkBlendAttachments.push_back(colorBlendAttachment);
        }

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = vkBlendAttachments.size();
        colorBlending.pAttachments = vkBlendAttachments.data();
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional

        return colorBlending;
    }

    VkPipelineColorBlendStateCreateInfo VulkanPipelineManager::CreateColorBlendState() {
        //
        return {};
    }

    VkPipelineViewportStateCreateInfo VulkanPipelineManager::CreateViewportState(
        const Rect &rect, VkViewport &viewport, VkRect2D &scissor) {
        viewport.x = rect.x;
        viewport.y = rect.y;
        viewport.width = rect.width;
        viewport.height = rect.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        scissor.offset = {.x = 0, .y = 0};
        scissor.extent = VkExtent2D(rect.width, rect.height);

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.pNext = nullptr;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        return viewportState;
    }

    VkPipelineDepthStencilStateCreateInfo VulkanPipelineManager::CreateDepthStencilState() {
        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.minDepthBounds = 0.0f; // Optional
        depthStencil.maxDepthBounds = 1.0f; // Optional
        depthStencil.stencilTestEnable = VK_FALSE;
        depthStencil.front = {}; // Optional
        depthStencil.back = {}; // Optional
        return depthStencil;
    }

    VkPipelineLayout VulkanPipelineManager::CreatePipelineLayout(
        const std::vector<VkDescriptorSetLayout> &descriptorSetLayouts) const {
        VkPipelineLayout pipelineLayout;
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = descriptorSetLayouts.size(); // Optional
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data(); // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

        if (const VkResult result = vkCreatePipelineLayout(logicalDevice.GetDevice(), &pipelineLayoutInfo, nullptr,
                                                           &pipelineLayout);
            result != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
        return pipelineLayout;
    }

    std::vector<VkPipelineShaderStageCreateInfo> VulkanPipelineManager::CreateShaderStages(
        const std::vector<ShaderHandle> &shaderHandles) const {
        std::vector<VkPipelineShaderStageCreateInfo> shaderStageInfos;
        shaderStageInfos.reserve(shaderHandles.size());

        for (const auto shaderHandle: shaderHandles) {
            VulkanShader shader = shaderManager.GetShader(shaderHandle);

            const VkShaderModule shaderModule = shader.VkShader;
            VkShaderStageFlagBits stage = {};

            switch (shader.Type) {
                case ShaderType::Vertex:
                    stage = VK_SHADER_STAGE_VERTEX_BIT;
                    break;
                case ShaderType::Fragment:
                    stage = VK_SHADER_STAGE_FRAGMENT_BIT;
                    break;
            }

            VkPipelineShaderStageCreateInfo shaderStageInfo{};
            shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStageInfo.stage = stage;
            shaderStageInfo.module = shaderModule;
            shaderStageInfo.pName = "main";

            shaderStageInfos.push_back(shaderStageInfo);
        }

        return shaderStageInfos;
    }

    std::vector<VkPipelineShaderStageCreateInfo> VulkanPipelineManager::CreateShaderStages(
        const GraphicsVulkanShader &shader) const {
        std::vector<VkPipelineShaderStageCreateInfo> shaderStageInfos;
        shaderStageInfos.reserve(2);

        VkPipelineShaderStageCreateInfo vShaderStageInfo{};
        vShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vShaderStageInfo.module = shader.VertexShader.VkShader;
        vShaderStageInfo.pName = "main";
        shaderStageInfos.push_back(vShaderStageInfo);

        VkPipelineShaderStageCreateInfo fShaderStageInfo{};
        fShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fShaderStageInfo.module = shader.FragmentShader.VkShader;
        fShaderStageInfo.pName = "main";
        shaderStageInfos.push_back(fShaderStageInfo);
        return shaderStageInfos;
    }

    VkPipeline VulkanPipelineManager::CreateGraphicsPipeline(
        const VkPipelineShaderStageCreateInfo *shaderStages,
        const uint32_t shaderStageCount,
        const VkPipelineVertexInputStateCreateInfo &vertexInputInfo,
        const VkPipelineInputAssemblyStateCreateInfo &inputAssembly,
        const VkPipelineViewportStateCreateInfo &viewportState,
        const VkPipelineRasterizationStateCreateInfo &rasterizer,
        const VkPipelineMultisampleStateCreateInfo &multisampling,
        const VkPipelineColorBlendStateCreateInfo &colorBlending,
        const VkPipelineDynamicStateCreateInfo &dynamicState,
        const VkPipelineLayout pipelineLayout,
        const VkRenderPass renderPass
    ) const {
        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = shaderStageCount;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = nullptr; // Optional
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipelineInfo.basePipelineIndex = -1; // Optional

        VkPipeline graphicsPipeline;

        if (const VkResult result = vkCreateGraphicsPipelines(logicalDevice.GetDevice(), VK_NULL_HANDLE, 1,
                                                              &pipelineInfo,
                                                              nullptr, &graphicsPipeline);
            result != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        return graphicsPipeline;
    }

    VkPipelineColorBlendAttachmentState VulkanPipelineManager::MapBlendMode(BlendMode mode) {
        VkPipelineColorBlendAttachmentState vkAttachment{};

        vkAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        switch (mode) {
            case BlendMode::Opaque: {
                vkAttachment.blendEnable = VK_FALSE;
                break;
            }

            case BlendMode::AlphaBlend: {
                vkAttachment.blendEnable = VK_TRUE;

                vkAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
                vkAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
                vkAttachment.colorBlendOp = VK_BLEND_OP_ADD;

                vkAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
                vkAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
                vkAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
                break;
            }

            case BlendMode::Additive: {
                vkAttachment.blendEnable = VK_TRUE;

                vkAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
                vkAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
                vkAttachment.colorBlendOp = VK_BLEND_OP_ADD;

                vkAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
                vkAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
                vkAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
                break;
            }

            default:
                throw std::runtime_error("Unsupported blend mode!");
        }

        return vkAttachment;
    }

    VkDynamicState VulkanPipelineManager::MapDynamicState(const DynamicState state) {
        switch (state) {
            case DynamicState::VIEWPORT: {
                return VK_DYNAMIC_STATE_VIEWPORT;
            }
            case DynamicState::SCISSOR: {
                return VK_DYNAMIC_STATE_SCISSOR;
            }
            case DynamicState::DEPTH_BIAS: {
                return VK_DYNAMIC_STATE_DEPTH_BIAS;
            }
            case DynamicState::LINE_WIDTH: {
                return VK_DYNAMIC_STATE_LINE_WIDTH;
            }
            case DynamicState::BLEND_CONSTANTS: {
                return VK_DYNAMIC_STATE_BLEND_CONSTANTS;
            }
            case DynamicState::DEPTH_BOUNDS: {
                return VK_DYNAMIC_STATE_DEPTH_BOUNDS;
            }
            case DynamicState::STENCIL_COMPARE_MASK: {
                return VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK;
            }
            case DynamicState::STENCIL_WRITE_MASK: {
                return VK_DYNAMIC_STATE_STENCIL_WRITE_MASK;
            }
            case DynamicState::STENCIL_REFERENCE: {
                return VK_DYNAMIC_STATE_STENCIL_REFERENCE;
            }
            default: {
                throw std::runtime_error("Unsupported dynamic state!");
            }
        }
    }

    VkFormat VulkanPipelineManager::MapImageFormat(const ImageFormat imageFormat) {
        switch (imageFormat) {
            case ImageFormat::RGBA8_Srgb: {
                return VK_FORMAT_R8G8B8A8_SRGB;
                break;
            }
            case ImageFormat::BGRA8_Srgb: {
                return VK_FORMAT_R8G8B8A8_SRGB;
            }
            case ImageFormat::D32_Sfloat_S8_Uint: {
                return VK_FORMAT_D32_SFLOAT_S8_UINT;
                break;
            }
            case ImageFormat::D24_Unorm_S8_Uint: {
                return VK_FORMAT_D24_UNORM_S8_UINT;
                break;
            }
            case ImageFormat::D32_Sfloat: {
                return VK_FORMAT_D32_SFLOAT;
                break;
            }
            case ImageFormat::Undefined: {
                return VK_FORMAT_UNDEFINED;
                break;
            }
            default: {
                throw std::runtime_error("Unsupported image format!");
            }
        }
    }
} // Rendering
