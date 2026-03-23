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
        src/graphics/Triangle.cpp
        src/graphics/Triangle.h
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
        src/graphics/SimpleMaterial.cpp
        src/graphics/SimpleMaterial.h
        src/math/converter/TypesConverterToGlad.cpp
        src/math/converter/TypesConverterToGlad.h
        src/utils/FileLoader.cpp
        src/utils/FileLoader.h
        src/graphics/RenderContext.cpp
        src/graphics/RenderContext.h
        src/math/Color.cpp
        src/math/Color.h
)

target_include_directories(MaxEngine PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(MaxEngine PRIVATE
        glfw
        opengl32
)

add_definitions(-DSHADERS_ROOT="${CMAKE_SOURCE_DIR}")