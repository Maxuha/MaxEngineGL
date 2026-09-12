//
// Created by zykov on 4/8/2026.
//

#include "ShaderImporter.h"
#include "../FileReader.h"
#include "../../renderer/domain/Shader.h"

ShaderImporter::~ShaderImporter() {
}

Shader* ShaderImporter::Import(const std::string &shaderName) {
    const std::string vSource = FileReader::ReadFileString((std::string(ASSETS_ROOT) + "/shaders/" + shaderName + ".vert").c_str());
    const std::string fSource = FileReader::ReadFileString((std::string(ASSETS_ROOT) + "/shaders/" + shaderName + ".frag").c_str());

    const char* vCode = vSource.c_str();
    const char* fCode = fSource.c_str();

    Rendering::VertexAttribute position_attribute;
    position_attribute.format = Rendering::VertexFormat::Float3;
    position_attribute.location = 0;
    position_attribute.offset = 0;
    position_attribute.bufferIndex = 0;

    Rendering::VertexAttribute normal_attribute;
    normal_attribute.format = Rendering::VertexFormat::Float3;
    normal_attribute.location = 1;
    normal_attribute.offset = offsetof(Vertex, normal);
    normal_attribute.bufferIndex = 0;

    Rendering::VertexAttribute texCoords_attribute;
    texCoords_attribute.format = Rendering::VertexFormat::Float2;
    texCoords_attribute.location = 2;
    texCoords_attribute.offset = offsetof(Vertex, texCoords);
    texCoords_attribute.bufferIndex = 0;

    Rendering::VertexBinding buffer_binding;
    buffer_binding.bufferIndex = 0;
    buffer_binding.stride = sizeof(Vertex);
    buffer_binding.stepRate = Rendering::VertexStepRate::PerVertex;

    Rendering::VertexLayout vertex_layout;
    vertex_layout.attributes.push_back(position_attribute);
    vertex_layout.attributes.push_back(normal_attribute);
    vertex_layout.attributes.push_back(texCoords_attribute);
    vertex_layout.bindings.push_back(buffer_binding);
    //
    // Rendering::DescriptorSetLayoutBinding descriptor_set_layout_binding;
    // descriptor_set_layout_binding.binding = 0;
    // descriptor_set_layout_binding.type = Rendering::ResourceType::UniformBuffer;
    // descriptor_set_layout_binding.descriptorCount = 1;
    // descriptor_set_layout_binding.stageFlags = 0;

    Rendering::PipelineStateDesc pipeline;
    pipeline.VertexLayout = vertex_layout;
    pipeline.DepthStencilState.depthTestEnable = true;
    pipeline.DepthStencilState.depthWriteEnable = true;
    pipeline.RasterizerState.cullEnable = true;
    pipeline.RasterizerState.cullMode = Rendering::CullMode::Front;
    pipeline.RasterizerState.fillMode = Rendering::FillMode::Solid;



    // Rendering::ShaderInput vertex;
    // vertex.Type = Rendering::ShaderType::Vertex;
    // vertex.Source = vCode;
    //
    // Rendering::ShaderInput fragment;
    // fragment.Type = Rendering::ShaderType::Fragment;
    // fragment.Source = fCode;
    //
    // const std::vector shaders = { vertex, fragment };
    //
    // DIContainer::GetInstance().Get<Rendering::IRenderer>()->CreatePipeline(shaders, pipeline);

    const auto shader = new Shader(vCode, fCode);

    return shader;
}

Shader * ShaderImporter::Import(const Vert &vert, const Frag &frag) {
    return nullptr;
}
