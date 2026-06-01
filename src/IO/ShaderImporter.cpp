//
// Created by zykov on 4/8/2026.
//

#include "ShaderImporter.h"
#include "FileReader.h"
#include "../di/DIContainer.h"
#include "../graphics/Shader.h"
#include "../renderer/manager/IPipelineManager.h"
#include "../renderer/manager/PipelineManager.h"

Shader* ShaderImporter::Import(const std::string &shaderName) {
    const std::string vSource = FileReader::ReadFileString((std::string(ASSETS_ROOT) + "/shaders/" + shaderName + ".vert").c_str());
    const std::string fSource = FileReader::ReadFileString((std::string(ASSETS_ROOT) + "/shaders/" + shaderName + ".frag").c_str());

    const char* vCode = vSource.c_str();
    const char* fCode = fSource.c_str();

   // const auto shaderId = DIContainer::GetInstance().Get<Rendering::IRenderDevice>()->CreateShader(vCode, fCode);;

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

    Rendering::PipelineStateDesc pipeline;
    pipeline.vertexShaderSource = vSource;
    pipeline.fragmentShaderSource = fSource;
    pipeline.vertexLayout = vertex_layout;
    pipeline.depthStencilState.depthTestEnable = true;
    pipeline.depthStencilState.depthWriteEnable = true;
    pipeline.rasteizerState.cullEnable = true;
   // pipeline.shaderId = shaderId;

    const auto shader = new Shader(vCode, fCode);
   // shader->pso = DIContainer::GetInstance().Get<Rendering::IRenderDevice>()->CreatePipelineState(pipeline);
    Rendering::IPipelineManager* pipeline_manager = new Rendering::PipelineManager();

    Rendering::ShaderInput shader1;
    shader1.Type = Rendering::ShaderType::Vertex;
    shader1.Source = vCode;

    Rendering::ShaderInput shader2;
    shader2.Type = Rendering::ShaderType::Fragment;
    shader2.Source = fCode;

    std::vector shaders = { shader1, shader2 };

    Rendering::RenderState renderState;

    //shader->pipeline = pipeline_manager->GetOrCreatePipeline(shaders, renderState);
    shader->pipelineHandle = DIContainer::GetInstance().Get<Rendering::IRenderer>()->CreatePipeline(shaders, renderState);

    return shader;
}
