//
// Created by zykov on 6/2/2026.
//

#include "SpirvImporter.h"

// #include "ShaderCompiler.h"
#include "../FileReader.h"
// #include "spirv_glsl.hpp"
#include "../../renderer/domain/Shader.h"


Shader *SpirvImporter::Import(const std::string &shaderName) {
    const std::string vSource = FileReader::ReadFileString((std::string(ASSETS_ROOT) + "/shaders/" + shaderName + ".vert").c_str());
    const std::string fSource = FileReader::ReadFileString((std::string(ASSETS_ROOT) + "/shaders/" + shaderName + ".frag").c_str());

    const char* vCode = vSource.c_str();
    const char* fCode = fSource.c_str();


  //  ShaderCompiler compiler;

    // std::vector<uint32_t> vertSPV = compiler.CompileGLSLToSPIRV(vCode, shaderc_vertex_shader, "main.vert");
    // std::vector<uint32_t> fragSPV = compiler.CompileGLSLToSPIRV(fCode, shaderc_fragment_shader, "main.frag");

    // spirv_cross::CompilerGLSL glslVert(std::move(vertSPV));
    // spirv_cross::CompilerGLSL glslFrag(std::move(fragSPV));

    // spirv_cross::CompilerGLSL::Options glslOptions;
    // glslOptions.version = 460;
    // glslVert.set_common_options(glslOptions);
    // glslFrag.set_common_options(glslOptions);
    //
    // std::string openGLVertCode = glslVert.compile();
    // std::string openGLFragCode = glslFrag.compile();


    return nullptr;
}
