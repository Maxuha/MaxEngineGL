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

add_executable(MaxEngine main.cpp src/glad.c)

target_include_directories(MaxEngine PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(MaxEngine PRIVATE
        glfw
        opengl32
)