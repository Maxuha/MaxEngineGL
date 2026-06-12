//
// Created by zykov on 3/17/2026.
//

#include "Scene.h"
#include "di/DIContainer.h"
#include "gameObject/primitives/Cube.h"

using namespace Rendering;

class AssetManager;

void ErrorHandle(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message,
                 const void *user_param) {
    // ?????????? ?????????????? ??????????? (?? ???????)
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;

    std::cerr << "--- OpenGL Debug Message ---" << std::endl;
    std::cerr << "Source: ";
    switch (source) {
        case GL_DEBUG_SOURCE_API: std::cerr << "API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: std::cerr << "Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cerr << "Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY: std::cerr << "Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION: std::cerr << "Application";
            break;
        case GL_DEBUG_SOURCE_OTHER: std::cerr << "Other";
            break;
        default: ;
    }
    std::cerr << std::endl;

    std::cerr << "Type: ";
    switch (type) {
        case GL_DEBUG_TYPE_ERROR: std::cerr << "ERROR";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cerr << "Deprecated Behavior";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: std::cerr << "Undefined Behavior";
            break;
        case GL_DEBUG_TYPE_PORTABILITY: std::cerr << "Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE: std::cerr << "Performance";
            break;
        case GL_DEBUG_TYPE_MARKER: std::cerr << "Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP: std::cerr << "Push Group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP: std::cerr << "Pop Group";
            break;
        case GL_DEBUG_TYPE_OTHER: std::cerr << "Other";
            break;
        default: ;
    }
    std::cerr << std::endl;

    std::cerr << "Severity: ";
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH: std::cerr << "HIGH";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM: std::cerr << "MEDIUM";
            break;
        case GL_DEBUG_SEVERITY_LOW: std::cerr << "LOW";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cerr << "Notification";
            break;
        default: ;
    }
    std::cerr << std::endl;

    std::cerr << "Message: " << message << std::endl;
    std::cerr << "-----------------------------" << std::endl;

    // ???? ??? ??????????? ??????, ????? ??????? ?????????? ??? ?????????
    if (severity == GL_DEBUG_SEVERITY_HIGH) {
        // __debugbreak(); // ??? MSVC
    }
}

void Scene::Init() const {
    for (GameObject *obj: gameObjects) {
        obj->Start();
    }

    //depthShader = DIContainer::GetInstance().Get<AssetManager>()->Import<Shader>("depth/depth");
}

void Scene::Update(const double delta_time) {
    camera->Update(delta_time);

    gameObjects[5]->GetComponent<Transform>()->RotateYaw(30.0f * delta_time);
    gameObjects[10]->GetComponent<Transform>()->RotateYaw(30.0f * delta_time);
    //gameObjects[10]->GetComponent<Transform>()->GetChild(0)->GetChild(0)->GetChild(0)->RotateYaw(-30.0f * delta_time);

    DIContainer::GetInstance().Get<IRenderer>()->BeginFrame(*camera);
    DIContainer::GetInstance().Get<IRenderer>()->Submit(*this);
    DIContainer::GetInstance().Get<IRenderer>()->Render();
    DIContainer::GetInstance().Get<IRenderer>()->EndFrame();
}

void Scene::Add(GameObject *gameObject) {
    gameObjects.push_back(gameObject);
}
