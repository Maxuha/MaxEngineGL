cmake_minimum_required(VERSION 4.1)
project(MaxEngine)

set(CMAKE_CXX_STANDARD 26)

include(FetchContent)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)

FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v6.0.4
)

FetchContent_MakeAvailable(assimp)

add_executable(MaxEngine main.cpp include/glad.c
        src/math/Vector3.cpp
        src/math/Vector3.h
        src/math/Matrix4x4.cpp
        src/math/Matrix4x4.h
        src/graphics/Mesh.cpp
        src/graphics/Mesh.h
        src/graphics/MeshPrimitives.cpp
        src/graphics/MeshPrimitives.h
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
        src/graphics/Shader.cpp
        src/graphics/Shader.h
        src/graphics/Material.cpp
        src/graphics/Material.h
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
        src/graphics/Texture.cpp
        src/graphics/Texture.h
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
        src/IO/ShaderImporter.cpp
        src/IO/ShaderImporter.h
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
        src/graphics/Model.cpp
        src/graphics/Model.h
        src/renderer/Renderer.cpp
        src/renderer/Renderer.h
        src/renderer/IRenderer.h
        src/renderer/DrawCall.h
        src/renderer/RenderQueue.cpp
        src/renderer/RenderQueue.h
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
        src/renderer/domain/buffer/frame/FrameBuffer.cpp
        src/renderer/domain/buffer/frame/FrameBuffer.h
        src/renderer/domain/GLTexture.cpp
        src/renderer/domain/GLTexture.h
        src/renderer/pass/IRenderPass.h
        src/renderer/pass/ShadowRenderPass.cpp
        src/renderer/pass/ShadowRenderPass.h
        src/renderer/pass/MainRenderPass.cpp
        src/renderer/pass/MainRenderPass.h
        src/graphics/IShader.h
        src/graphics/ITexture.h
        src/renderer/domain/struct/Mesh.h
        src/renderer/domain/struct/BufferId.h
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
        src/renderer/domain/GLCommandBuffer.cpp
        src/renderer/domain/GLCommandBuffer.h
        src/renderer/domain/GLPipeline.cpp
        src/renderer/domain/GLPipeline.h
        src/renderer/manager/IPipelineManager.h
        src/renderer/manager/PipelineManager.cpp
        src/renderer/manager/PipelineManager.h
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
)

target_include_directories(MaxEngine PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(MaxEngine PRIVATE
        glfw
        opengl32
        assimp
)

if(MINGW)
    target_link_options(MaxEngine PRIVATE -Wl,--allow-multiple-definition)
endif()

add_definitions(-DROOT="${CMAKE_SOURCE_DIR}")
add_definitions(-DASSETS_ROOT="${CMAKE_SOURCE_DIR}/assets")

file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

option(BUILD_SHARED_LIBS "Build shared libraries" ON)

set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)