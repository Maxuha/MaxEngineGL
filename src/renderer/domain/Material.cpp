//
// Created by zykov on 3/19/2026.
//

#include "Material.h"

#include "../RendererContext.h"

Material::Material(IShader *shader, std::string name) : shader(shader), name(name) {
    cpuBuffer.resize(32, 0);

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
    pipeline.vertexLayout = vertex_layout;
    pipeline.depthStencilState.depthTestEnable = true;
    pipeline.depthStencilState.depthWriteEnable = true;
    pipeline.rasterizerState.cullEnable = true;
    pipeline.rasterizerState.cullMode = Rendering::CullMode::Front;
    pipeline.rasterizerState.fillMode = Rendering::FillMode::Solid;

    commonPropertiesMeta = shader->GetProperties();

    const auto shaderProps = shader->GetProperties();

    // for (const auto& [name, property] : shaderProps) {
    //     properties[name] =
    // }

    id = Rendering::RendererContext::GetContext()->CreateMaterial(*shader, pipeline);
}

MaterialHandle Material::GetId() const {
    return id;
}

IShader *Material::GetShader() const {
    return shader;
}

void Material::Apply(const std::string &slot)  {
    if (const auto it = propertiesMeta.find(slot); it != propertiesMeta.end()) {
        const ShaderMetaProperty& meta = it->second;
        Rendering::RendererContext::GetContext()->UpdateMaterialData(id, meta.offset, meta.size, cpuBuffer.data() + meta.offset);
    }
}

void Material::Apply(const ShaderProperty &slot) {
    if (const auto it = commonPropertiesMeta.find(slot); it != commonPropertiesMeta.end()) {
        const ShaderMetaProperty& meta = it->second;
        Rendering::RendererContext::GetContext()->UpdateMaterialData(id, meta.offset, meta.size, cpuBuffer.data() + meta.offset);
    }
}
