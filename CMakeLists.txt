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

add_executable(MaxEngine main.cpp src/glad.c
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
        src/gameObject/light/Light.cpp
        src/gameObject/light/Light.h
        src/gameObject/light/DirectionalLight.cpp
        src/gameObject/light/DirectionalLight.h
        src/graphics/Vertex.cpp
        src/graphics/Vertex.h
        src/Scene.cpp
        src/Scene.h
        src/Window.cpp
        src/Window.h
        src/graphics/Shader.cpp
        src/graphics/Shader.h
        src/graphics/Material.cpp
        src/graphics/Material.h
        src/graphics/DefaultMaterial.cpp
        src/graphics/DefaultMaterial.h
        src/graphics/RenderContext.cpp
        src/graphics/RenderContext.h
        src/math/Color.cpp
        src/math/Color.h
        src/IO/FileReader.cpp
        src/IO/FileReader.h
        src/IO/ObjImporter.cpp
        src/IO/ObjImporter.h
        src/IO/ModelImporter.h
        src/math/AABB.cpp
        src/math/AABB.h
        src/graphics/buffers/VertexArray.cpp
        src/graphics/buffers/VertexArray.h
        src/graphics/buffers/VertexBuffer.cpp
        src/graphics/buffers/VertexBuffer.h
        src/graphics/buffers/IndexBuffer.cpp
        src/graphics/buffers/IndexBuffer.h
        src/IO/model/VertexSet.h
        src/IO/model/IndexSet.h
        src/Engine.cpp
        src/Engine.h
        src/IO/TextureImporter.cpp
        src/IO/TextureImporter.h
        src/graphics/Texture.cpp
        src/graphics/Texture.h
        include/stb_image.h
        src/math/Vector2.cpp
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
        src/gameObject/light/PointLight.cpp
        src/gameObject/light/PointLight.h
        src/gameObject/light/SpotLight.cpp
        src/gameObject/light/SpotLight.h
        src/gameObject/light/AmbientLight.cpp
        src/gameObject/light/AmbientLight.h
        src/gameObject/light/LocationalLight.cpp
        src/gameObject/light/LocationalLight.h
)

target_include_directories(MaxEngine PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(MaxEngine PRIVATE
        glfw
        opengl32
)

add_definitions(-DROOT="${CMAKE_SOURCE_DIR}")
add_definitions(-DASSETS_ROOT="${CMAKE_SOURCE_DIR}/assets")

file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})