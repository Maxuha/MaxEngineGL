//
// Created by zykov on 4/8/2026.
//

#include "ShaderImporter.h"

#include <ranges>

#include "../FileReader.h"
#include "../../../cmake-build-debug/_deps/spirv_reflect-src/spirv_reflect.h"
#include "../../renderer/domain/Shader.h"

ShaderImporter::~ShaderImporter() {
}

Shader* ShaderImporter::Import(const std::string &shaderName, const std::unordered_map<std::string, std::string>& params) {
    const std::vector<char> vSPV = FileReader::ReadFileBytes(
     std::string(ASSETS_ROOT) + "/shaders/" + shaderName + "-v" + ".spv");
    const std::vector<char> fSPV = FileReader::ReadFileBytes(
        std::string(ASSETS_ROOT) + "/shaders/" + shaderName + "-f" + ".spv");

    ShaderDesc desc;
    desc.vertexCode = vSPV;
    desc.fragmentCode = fSPV;

    if (params.contains("cullMode")) {
        const std::string cullMode = params.at("cullMode");

        if (cullMode == "Front") desc.CullMode = Rendering::CullMode::Front;
        if (cullMode == "Back") desc.CullMode = Rendering::CullMode::Back;
    }

    if (params.contains("isDepthOnly")) {
        const std::string isDepthOnly = params.at("isDepthOnly");
        desc.IsDepthOnly = isDepthOnly == "true";
    }

    reflectShader(desc);

    const auto shader = new Shader(vSPV, fSPV, desc);

    return shader;
}

void ShaderImporter::reflectShader(ShaderDesc& desc) {
    const std::vector<char>& vCode = desc.vertexCode;
    const std::vector<char>& fCode = desc.fragmentCode;

    std::unordered_map<uint32_t, std::unordered_map<uint32_t, Rendering::ResourceBindingDesc>> aggregatedSets;

    if (!vCode.empty()) {
        reflectStage(vCode, Rendering::ShaderStageFlags::Vertex, aggregatedSets);
    }

    if (!fCode.empty()) {
        reflectStage(fCode, Rendering::ShaderStageFlags::Fragment, aggregatedSets);
    }

    // 3. ????????? ?????????????? ?????? ? ????????? ????????? ShaderDesc
    uint32_t maxSetIndex = 0;
    for (const auto &setIndex: aggregatedSets | std::views::keys) {
        maxSetIndex = std::max(maxSetIndex, setIndex);
    }

    // ???????? ?????? ??? ??? ????, ?????? ?? ?????????????,
    // ????? ?????? ? ??????? ?????? ?????????????? set_index ? ???????!
    desc.resourceSetLayouts.resize(maxSetIndex + 1);

    for (auto& [setIndex, bindingsMap] : aggregatedSets) {
        Rendering::ResourceSetLayoutDesc& layout = desc.resourceSetLayouts[setIndex]; // ????????? ?????
        layout.set_index = setIndex;
        for (auto &bindingDesc: bindingsMap | std::views::values) {
            layout.bindings.push_back(bindingDesc);
        }

        // ????????? ???????? ?????? ????
        std::ranges::sort(layout.bindings, [](const Rendering::ResourceBindingDesc& a, const Rendering::ResourceBindingDesc& b) {
            return a.binding_slot < b.binding_slot;
        });
    }

    // for (auto& [setIndex, bindingsMap] : aggregatedSets) {
    //     Rendering::ResourceSetLayoutDesc layout;
    //     layout.set_index = setIndex;
    //
    //     for (auto& [bindingSlot, bindingDesc] : bindingsMap) {
    //         layout.bindings.push_back(bindingDesc);
    //     }
    //
    //     std::ranges::sort(layout.bindings, [](const Rendering::ResourceBindingDesc& a, const Rendering::ResourceBindingDesc& b) {
    //         return a.binding_slot < b.binding_slot;
    //     });
    //
    //     desc.resourceSetLayouts.push_back(layout);
    // }
    //
    // std::ranges::sort(desc.resourceSetLayouts, [](const Rendering::ResourceSetLayoutDesc& a, const Rendering::ResourceSetLayoutDesc& b) {
    //     return a.set_index < b.set_index;
    // });

    // return desc;
}

void ShaderImporter::reflectStage(const std::vector<char> &code, const Rendering::ShaderStageFlags stage,
                                  std::unordered_map<uint32_t, std::unordered_map<uint32_t, Rendering::ResourceBindingDesc>> &aggregatedSets) {
    SpvReflectShaderModule module;
    SpvReflectResult result = spvReflectCreateShaderModule(code.size(), code.data(), &module);

    if (result != SPV_REFLECT_RESULT_SUCCESS) {
        return;
    }

    uint32_t descriptorCount = 0;
    spvReflectEnumerateDescriptorBindings(&module, &descriptorCount, nullptr);

    std::vector<SpvReflectDescriptorBinding*> bindings(descriptorCount);
    spvReflectEnumerateDescriptorBindings(&module, &descriptorCount, bindings.data());

    for (const auto* b : bindings) {
        uint32_t setIdx = b->set;
        uint32_t bindingSlot = b->binding;

        if (aggregatedSets[setIdx].contains(bindingSlot)) {
            aggregatedSets[setIdx][bindingSlot].stage_flags |= stage;
        } else {
            Rendering::ResourceBindingDesc bindingDesc{};
            bindingDesc.binding_slot = bindingSlot;
            bindingDesc.type = MapDescriptorType(b->descriptor_type);
            bindingDesc.stage_flags = stage;

            uint32_t count = 1;
            for (uint32_t i = 0; i < b->array.dims_count; ++i) {
                count *= b->array.dims[i];
            }
            bindingDesc.count = count;

            aggregatedSets[setIdx][bindingSlot] = bindingDesc;
        }
    }

    spvReflectDestroyShaderModule(&module);
}

Rendering::ResourceType ShaderImporter::MapDescriptorType(const SpvReflectDescriptorType type) {
    switch (type) {
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
            return Rendering::ResourceType::UniformBuffer;
        case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            return Rendering::ResourceType::SampledImage;
        default:
            return Rendering::ResourceType::UniformBuffer;
    }
}
