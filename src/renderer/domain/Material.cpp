//
// Created by zykov on 3/19/2026.
//

#include "Material.h"

#include "../RenderContext.h"
#include "../../di/DIContainer.h"
#include "../../math/Vertex.h"

Material::Material(const MaterialHandle handle) : id(handle), shader(nullptr) {
}

Material::Material(const Shader *shader, const std::string &name) : name(name), shader(nullptr) {
     cpuBuffer.resize(32, 0);
    //
    // Rendering::VertexAttribute position_attribute;
    // position_attribute.format = Rendering::VertexFormat::Float3;
    // position_attribute.location = 0;
    // position_attribute.offset = 0;
    // position_attribute.bufferIndex = 0;
    //
    // Rendering::VertexAttribute normal_attribute;
    // normal_attribute.format = Rendering::VertexFormat::Float3;
    // normal_attribute.location = 1;
    // normal_attribute.offset = offsetof(Vertex, normal);
    // normal_attribute.bufferIndex = 0;
    //
    // Rendering::VertexAttribute texCoords_attribute;
    // texCoords_attribute.format = Rendering::VertexFormat::Float2;
    // texCoords_attribute.location = 2;
    // texCoords_attribute.offset = offsetof(Vertex, texCoords);
    // texCoords_attribute.bufferIndex = 0;
    //
    // Rendering::VertexBinding buffer_binding;
    // buffer_binding.bufferIndex = 0;
    // buffer_binding.stride = sizeof(Vertex);
    // buffer_binding.stepRate = Rendering::VertexStepRate::PerVertex;
    //
    // Rendering::VertexLayout vertex_layout;
    // vertex_layout.attributes.push_back(position_attribute);
    // vertex_layout.attributes.push_back(normal_attribute);
    // vertex_layout.attributes.push_back(texCoords_attribute);
    // vertex_layout.bindings.push_back(buffer_binding);
    // //
    // // Rendering::DescriptorSetLayoutBinding descriptor_set_layout_binding;
    // // descriptor_set_layout_binding.binding = 0;
    // // descriptor_set_layout_binding.type = Rendering::ResourceType::UniformBuffer;
    // // descriptor_set_layout_binding.descriptorCount = 1;
    // // descriptor_set_layout_binding.stageFlags = 0;
    //
    // Rendering::PipelineStateDesc pipeline;
    // pipeline.VertexLayout = vertex_layout;
    // pipeline.DepthStencilState.depthTestEnable = true;
    // pipeline.DepthStencilState.depthWriteEnable = true;
    // pipeline.RasterizerState.cullEnable = true;
    // pipeline.RasterizerState.cullMode = Rendering::CullMode::Front;
    // pipeline.RasterizerState.fillMode = Rendering::FillMode::Solid;
    //
    // commonPropertiesMeta = shader->GetProperties();
    //
    // const auto shaderProps = shader->GetProperties();

    // for (const auto& [name, property] : shaderProps) {
    //     properties[name] =
    // }

     //id = Rendering::RenderContext::GetContext()->CreateMaterial(*shader, pipeline);
   // id = DIContainer::GetInstance().Get<Rendering::IRenderer>()->CreateMaterial(*shader, pipeline);

    Rendering::MaterialDesc desc;

    Rendering::ResourceBinding resourceBinding{};
    resourceBinding.slot = 2;
    resourceBinding.binding = 4;
    resourceBinding.type = Rendering::ResourceType::SampledImage;

    Rendering::ResourceBinding resourceBinding2{};
    resourceBinding2.slot = 2;
    resourceBinding2.binding = 5;
    resourceBinding2.type = Rendering::ResourceType::SampledImage;

    Rendering::ResourceBinding resourceBinding3{};
    resourceBinding3.slot = 2;
    resourceBinding3.binding = 3;
    resourceBinding3.type = Rendering::ResourceType::UniformBuffer;

    desc.resourceSet.bindings.push_back(resourceBinding);
    desc.resourceSet.bindings.push_back(resourceBinding2);
    desc.resourceSet.bindings.push_back(resourceBinding3);

    id = DIContainer::GetInstance().Get<Rendering::IRenderer>()->CreateMaterial(shader->GetHandle(), desc);

    dirtyFrames = 3;
}

MaterialHandle Material::GetId() const {
    return id;
}

IShader *Material::GetShader() const {
    return shader;
}

bool Material::IsDirty() const {
    return isDirty;
}

void Material::ClearDirty() {
    isDirty = false;
}

void Material::SetTexture(const uint32_t slot, Rendering::Texture &texture) {
    DIContainer::GetInstance().Get<Rendering::IRenderer>()->BindTextureToMaterial(*this, slot, texture);
}

MaterialHandle Material::GetMaterialHandle() const {
    return id;
}

std::string Material::GetName() const {
    return name;
}

void Material::Apply(const std::string &slot)  {
    if (const auto it = propertiesMeta.find(slot); it != propertiesMeta.end()) {
        const ShaderMetaProperty& meta = it->second;
        // Rendering::RenderContext::GetContext()->UpdateMaterialData(id, meta.offset, meta.size, cpuBuffer.data() + meta.offset);
    }
}

void Material::Apply(const ShaderProperty &slot) {
    if (const auto it = commonPropertiesMeta.find(slot); it != commonPropertiesMeta.end()) {
        const ShaderMetaProperty& meta = it->second;
        // Rendering::RenderContext::GetContext()->UpdateMaterialData(id, meta.offset, meta.size, cpuBuffer.data() + meta.offset);
    }
}
