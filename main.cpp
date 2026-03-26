#include <iostream>
#include <ostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "src/Scene.h"
#include "src/Window.h"
#include "src/gameObject/Camera.h"
#include "src/components/Transform.h"
#include "src/gameObject/primitives/Cube.h"
#include "src/math/Vector3.h"

class Camera;

int main() {
    int width = 1280, height = 720;

    auto* window = new Window(width, height);
    window->MakeWindow();

    auto* scene = new Scene(width, height);
    scene->Init();

    double last_time = glfwGetTime();
    double delta_time = 0;

    //Warning
    Vector3 directionRot = Vector3::Zero();

    float maxCamaraSpeed = 10.0f;
    float minCameraSpeed = 2.0f;
    float cameraSpeed = minCameraSpeed;

    while (!window->IsClosed()) {
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        delta_time = glfwGetTime() - last_time;
        last_time = glfwGetTime();

        scene->Render(delta_time);

        window->SwapBuffers();
        glfwPollEvents();

        // Camera translate
        auto direction = Vector3::Zero();

        if (window->GetInputKey(GLFW_KEY_LEFT_SHIFT)) {
            cameraSpeed = maxCamaraSpeed;
        } else {
            cameraSpeed = minCameraSpeed;
        }

        if (window->GetInputKey(GLFW_KEY_W)) {
            direction += Vector3::Forward();
        }
        if (window->GetInputKey(GLFW_KEY_S)) {
            direction -= Vector3::Forward();
        }
        if (window->GetInputKey(GLFW_KEY_A)) {
            direction -= Vector3::Right();
        }
        if (window->GetInputKey(GLFW_KEY_D)) {
            direction += Vector3::Right();
        }

        auto* cameraTransform = scene->camera->GetComponent<Transform>();
        if (window->GetInputKey(GLFW_KEY_W) ||
            window->GetInputKey(GLFW_KEY_S) ||
            window->GetInputKey(GLFW_KEY_A) ||
            window->GetInputKey(GLFW_KEY_D)) {

            cameraTransform->Translate(direction * cameraSpeed * delta_time);
        }

        double x, y;

        window->GetCursorPos(&x, &y);

        directionRot = Vector3(x - directionRot.x, y - directionRot.y, directionRot.z);

        float Yaw = directionRot.x * delta_time * 30;
        float Pitch = directionRot.y * delta_time * 30;

        if (Yaw != 0 || Pitch != 0) {
            cameraTransform = scene->camera->GetComponent<Transform>();
            cameraTransform->RotateYaw(Yaw);
            cameraTransform->RotatePitch(Pitch);
        }
        directionRot = Vector3(x, y, directionRot.z);
    }
    return 0;
}
