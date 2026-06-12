cmake_minimum_required(VERSION 4.1)
project(MaxEngine)

set(CMAKE_CXX_STANDARD 26)

include(FetchContent)

#glfw

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)

#assimp

FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v6.0.4
)

#spirv

FetchContent_Declare(
        spirv_cross
        GIT_REPOSITORY https://github.com/khronosgroup/spirv-cross
        GIT_TAG vulkan-sdk-1.4.350.0
)

set(SPIRV_CROSS_CLI OFF CACHE INTERNAL "")
set(SPIRV_CROSS_TESTS OFF CACHE INTERNAL "")
set(SPIRV_CROSS_ENABLE_MSL OFF CACHE INTERNAL "")
set(SPIRV_CROSS_ENABLE_HLSL OFF CACHE INTERNAL "")
set(SPIRV_CROSS_ENABLE_GLSL ON CACHE INTERNAL "")
set(SPIRV_CROSS_ENABLE_CPP OFF CACHE INTERNAL "")
set(SPIRV_CROSS_ENABLE_REFLECT OFF CACHE INTERNAL "")

##SPIRV-Headers
#
#FetchContent_Declare(
#        spirv-headers
#        GIT_REPOSITORY https://github.com/KhronosGroup/Spirv-headers
#        GIT_TAG        main
#)
#
##SPIRV-Tools
#
#FetchContent_Declare(
#        spirv-tools
#        GIT_REPOSITORY https://github.com/khronosGroup/SPIRV-Tools
#        GIT_TAG        main
#)
#
##gslang
#
#FetchContent_Declare(
#        glslang
#        GIT_REPOSITORY https://github.com/khronosGroup/glslang
#        GIT_TAG        main
#)
#
#set(SKIP_GLSLANG_INSTALL ON CACHE INTERNAL "")
#set(ENABLE_OPT OFF CACHE INTERNAL "")
#set(ALLOW_EXTERNAL_SPIRV_TOOLS ON CACHE INTERNAL "")
#
#set(SPIRV_TOOLS_INSTALL_DIR "${CMAKE_BINARY_DIR}/_deps/spirv_tools-src" CACHE INTERNAL "")
#
##shaderc
#
#FetchContent_Declare(
#        shaderc
#        GIT_REPOSITORY https://github.com/google/shaderc
#        GIT_TAG main
#        GIT_SUBMODULES ""
#)
#
#set(SHADERC_SKIP_TESTS ON CACHE INTERNAL "")
#set(SHADERC_SKIP_EXAMPLES ON CACHE INTERNAL "")
#set(SHADERC_SKIP_COPYRIGHT_CHECK ON CACHE INTERNAL "")

#FetchContent_MakeAvailable(glfw assimp spirv-headers spirv_cross  spirv-tools glslang shaderc)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw assimp spirv_cross)

add_executable(MaxEngine main.cpp include/glad.c
        src/math/Vector3.cpp
        src/math/Vector3.h
        src/math/Matrix4x4.cpp
        src/math/Matrix4x4.h
        src/renderer/domain/Mesh.cpp
        src/renderer/domain/Mesh.h
        src/renderer/domain/MeshPrimitives.cpp
        src/renderer/domain/MeshPrimitives.h
        src/gameObject/GameObject.cpp
        src/gameObject/GameObject.h
        src/components/Component.cpp
        src/components/Component.h
        src/components/Transform.cpp
        src/components/Transform.h
        src/gameObject/Camera.cpp
        src/gameObject/Camera.h
        src/components/MeshRenderer.cpp
        src/components/MeshRenderer.h
        src/gameObject/primitives/Cube.cpp
        src/gameObject/primitives/Cube.h
        src/components/light/Light.cpp
        src/components/light/Light.h
        src/components/light/DirectionalLight.cpp
        src/components/light/DirectionalLight.h
        src/math/Vertex.h
        src/Scene.cpp
        src/Scene.h
        src/Window.cpp
        src/Window.h
        src/renderer/domain/Shader.cpp
        src/renderer/domain/IShader.h
        src/renderer/domain/Material.cpp
        src/renderer/domain/Material.h
        src/math/Color.cpp
        src/math/Color.h
        src/IO/FileReader.cpp
        src/IO/FileReader.h
        src/IO/ObjImporter.cpp
        src/IO/ObjImporter.h
        src/IO/ModelImporter.h
        src/math/AABB.cpp
        src/math/AABB.h
        src/IO/model/VertexSet.h
        src/IO/model/IndexSet.h
        src/Engine.cpp
        src/Engine.h
        src/IO/TextureImporter.cpp
        src/IO/TextureImporter.h
        include/stb_image.h
        include/stb_image_write.h
        src/math/Vector2.h
        src/Demo.cpp
        src/Demo.h
        src/InputController.cpp
        src/InputController.h
        src/FPSCamera.cpp
        src/FPSCamera.h
        src/di/DIContainer.h
        src/IO/AssetManager.cpp
        src/IO/AssetManager.h
        src/IO/MeshAssetManager.cpp
        src/IO/MeshAssetManager.h
        src/IO/shader/ShaderImporter.cpp
        src/IO/shader/ShaderImporter.h
        src/math/Vector4.cpp
        src/math/Vector4.h
        src/math/Matrix3x3.cpp
        src/math/Matrix3x3.h
        src/components/physics/collision/BoxCollider.cpp
        src/components/physics/collision/BoxCollider.h
        src/physics/PhysicsEngine.cpp
        src/physics/PhysicsEngine.h
        src/di/DIContainer.cpp
        src/components/physics/collision/Collider.cpp
        src/components/physics/collision/Collider.h
        src/components/light/PointLight.cpp
        src/components/light/PointLight.h
        src/components/light/SpotLight.cpp
        src/components/light/SpotLight.h
        src/components/light/AmbientLight.cpp
        src/components/light/AmbientLight.h
        src/components/light/LocationalLight.cpp
        src/components/light/LocationalLight.h
        src/IO/AssimpImporter.cpp
        src/IO/AssimpImporter.h
        src/renderer/domain/Model.cpp
        src/renderer/domain/Model.h
        src/renderer/Renderer.cpp
        src/renderer/Renderer.h
        src/renderer/IRenderer.h
        src/renderer/DrawCall.h
        src/renderer/IRenderDevice.h
        src/renderer/gl/GLRenderDevice.cpp
        src/renderer/gl/GLRenderDevice.h
        src/utils/Hash.cpp
        src/utils/Hash.h
        src/renderer/CameraData.h
        src/renderer/AmbientLightData.h
        src/IScene.h
        src/components/light/LightFactory.cpp
        src/components/light/LightFactory.h
        src/renderer/LightRenderer.cpp
        src/renderer/LightRenderer.h
        src/renderer/ILightRenderer.h
        src/renderer/domain/buffer/BufferUsage.h
        src/renderer/domain/buffer/BufferDesc.h
        src/renderer/domain/buffer/BufferSlot.h
        src/renderer/domain/GLTexture.cpp
        src/renderer/domain/GLTexture.h
        src/renderer/domain/IShader.h
        src/renderer/domain/struct/BufferHandle.h
        src/renderer/domain/struct/MeshHandle.h
        src/renderer/domain/struct/MeshCreateDesc.h
        src/renderer/domain/struct/VertexLayout.h
        src/renderer/domain/struct/PipelineStateDesc.h
        src/renderer/domain/struct/Pipeline.h
        src/renderer/domain/struct/PipelineHandle.h
        src/renderer/domain/struct/ResourceSet.h
        src/renderer/gl/model/TextureData.h
        src/renderer/domain/IPipeline.h
        src/renderer/domain/ICommandBuffer.h
        src/renderer/gl/GLCommandBuffer.cpp
        src/renderer/gl/GLCommandBuffer.h
        src/renderer/domain/GLPipeline.cpp
        src/renderer/domain/GLPipeline.h
        src/renderer/domain/IResourceSet.h
        src/renderer/domain/GLResourceSet.cpp
        src/renderer/domain/GLResourceSet.h
        src/renderer/domain/IGLTexture.h
        src/renderer/domain/struct/IBuffer.h
        src/renderer/domain/struct/Buffer.cpp
        src/renderer/domain/struct/Buffer.h
        src/renderer/gl/desc/TextureCreateRequest.h
        src/IO/model/TextureAsset.h
        src/IWindow.h
        src/renderer/domain/struct/RenderConfig.h
        src/renderer/domain/IWindowContext.h
        src/IO/shader/IShaderImporter.h
        src/IO/shader/SpirvImporter.cpp
        src/IO/shader/SpirvImporter.h
        src/IO/shader/ShaderCompiler.cpp
        src/IO/shader/ShaderCompiler.h
        src/renderer/gl/model/PipelineData.h
        src/renderer/domain/struct/MaterialData.h
        src/IO/material/IMaterialImporter.h
        src/IO/material/MaterialImporter.cpp
        src/IO/material/MaterialImporter.h
        src/renderer/RendererContext.h
        src/renderer/gl/model/PipelineLayout.h
        src/renderer/gl/managers/BufferManager.cpp
        src/renderer/gl/managers/BufferManager.h
        src/renderer/gl/managers/BufferData.h
        src/renderer/gl/managers/BufferManager.cpp
)

target_include_directories(MaxEngine PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

target_precompile_headers(MaxEngine PRIVATE
        "$<$<COMPILE_LANGUAGE:CXX>:<vector>>"
        "$<$<COMPILE_LANGUAGE:CXX>:<string>>"
        "$<$<COMPILE_LANGUAGE:CXX>:<memory>>"
        "$<$<COMPILE_LANGUAGE:CXX>:<unordered_map>>"
        "$<$<COMPILE_LANGUAGE:CXX>:<functional>>"
        "$<$<COMPILE_LANGUAGE:CXX>:<cstdint>>"
        "$<$<COMPILE_LANGUAGE:CXX>:<glm/glm.hpp>>"
        "$<$<COMPILE_LANGUAGE:CXX>:<glm/gtc/matrix_transform.hpp>>"
        "$<$<COMPILE_LANGUAGE:CXX>:<glm/gtc/type_ptr.hpp>>"
        "$<$<COMPILE_LANGUAGE:CXX>:<glad/glad.h>>"
        "$<$<COMPILE_LANGUAGE:CXX>:<GLFW/glfw3.h>>"
)

set_target_properties(MaxEngine PROPERTIES UNITY_BUILD ON UNITY_BUILD_BATCH_SIZE 8)

target_link_libraries(MaxEngine PRIVATE
#        shaderc
        glfw
        opengl32
        assimp
        spirv-cross-core
        spirv-cross-glsl
)

if(MINGW)
    target_link_options(MaxEngine PRIVATE -Wl,--allow-multiple-definition)
endif()

add_definitions(-DROOT="${CMAKE_SOURCE_DIR}")
add_definitions(-DASSETS_ROOT="${CMAKE_SOURCE_DIR}/assets")

file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

option(BUILD_SHARED_LIBS "Build shared libraries" ON)