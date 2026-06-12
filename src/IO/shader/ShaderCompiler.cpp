//
// Created by zykov on 6/3/2026.
//

#include "ShaderCompiler.h"

// std::vector<uint32_t> ShaderCompiler::CompileGLSLToSPIRV(const std::string &sourceCode,
//     const shaderc_shader_kind shaderKind, const char *inputFileName) {
//     const shaderc::Compiler compiler;
//     shaderc::CompileOptions options;
//
//     options.SetOptimizationLevel(shaderc_optimization_level_performance);
//
//     options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_0);
//
//     const shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(
//         sourceCode,
//         shaderKind,
//         inputFileName,
//         options
//     );
//
//     if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
//         std::cerr << "?????? ?????????? Shaderc (" << inputFileName << "):\n"
//                   << result.GetErrorMessage() << std::endl;
//         return {};
//     }
//
//     return {result.cbegin(), result.cend()};
// }
