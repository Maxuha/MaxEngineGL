#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "src/components/Camera.h"
#include "src/components/MeshRenderer.h"
#include "src/components/Transform.h"
#include "src/gameObject/primitives/Cube.h"
#include "src/math/Vector3.h"

class Camera;

class Window {
private:
    GLFWwindow *window{};

public:
    Window(const int width, const int height) {
        this->width = width;
        this->height = height;
    }

    int width, height;

    void MakeWindow() {
        if (!glfwInit()) {
            std::cout << "Failed to init GLFW\n";
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(width, height, "Max Engine", nullptr, nullptr);
        if (!window) {
            std::cout << "Failed to create window\n";
            glfwTerminate();
        }

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
            std::cout << "Failed to initialize GLAD\n";
            glfwDestroyWindow(window);
            glfwTerminate();
        }

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }

        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, width, height);

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void SwapBuffers() {
        glfwSwapBuffers(window);
    }

    void DestroyWindow() {
        if (IsClosed()) return;

        glfwDestroyWindow(window);
        glfwTerminate();
    }

    bool GetInputKey(const int key) const {
        return glfwGetKey(window, key) == GLFW_PRESS;
    }

    void GetCursorPos(double *x, double *y) const {
        glfwGetCursorPos(window, x, y);
    }

    int IsClosed() const {
        return glfwWindowShouldClose(window);
    }
};

int main() {
    Window window = Window(1280, 720);
    window.MakeWindow();

    auto *camera = new Camera();
    auto cameraTransform = camera->AddComponent<Transform>();
    cameraTransform->position = {0.0, 1.0, -3.0};
    cameraTransform->forward = Vector3::Forward();
    cameraTransform->up = Vector3::Up();
    cameraTransform->right = Vector3::Right();
    camera->fov = 90;
    camera->near = 0.01;
    camera->far = 100;
    camera->aspectRatio = static_cast<float>(window.width) / static_cast<float>(window.height);

    //render cube
    auto Cube = Cube::BuildCube(camera);
    auto Cube2 = Cube::BuildCube(camera);

    double last_time = glfwGetTime();
    double delta_time = 0;

    //Warning
    Vector3 directionRot = Vector3::Zero();

    float maxCamaraSpeed = 10.0f;
    float minCameraSpeed = 2.0f;
    float cameraSpeed = minCameraSpeed;

    auto* mesh_renderer1 = Cube->GetComponent<MeshRenderer>();
    auto* transform1 = Cube->GetComponent<Transform>();
    auto* mesh_renderer2 = Cube2->GetComponent<MeshRenderer>();
    auto* transform2 = Cube2->GetComponent<Transform>();

    while (!window.IsClosed()) {
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

       transform1->Translate(Vector3::Forward() * 1 * delta_time);
      //  transform2->Translate(Vector3::Up() * cameraSpeed / 3 * delta_time);
        transform1->RotateYaw(30 * delta_time);

        mesh_renderer1->Render();
      //  mesh_renderer2->Render();

        window.SwapBuffers();
        glfwPollEvents();

        delta_time = glfwGetTime() - last_time;
        last_time = glfwGetTime();

        // Camera translate
        auto direction = Vector3::Zero();

        if (window.GetInputKey(GLFW_KEY_LEFT_SHIFT)) {
            cameraSpeed = maxCamaraSpeed;
        } else {
            cameraSpeed = minCameraSpeed;
        }

        if (window.GetInputKey(GLFW_KEY_W)) {
            direction += Vector3::Forward();
        }
        if (window.GetInputKey(GLFW_KEY_S)) {
            direction -= Vector3::Forward();
        }
        if (window.GetInputKey(GLFW_KEY_A)) {
            direction += Vector3::Right();
        }
        if (window.GetInputKey(GLFW_KEY_D)) {
            direction -= Vector3::Right();
        }

        auto* cameraTransform = camera->GetComponent<Transform>();
        if (window.GetInputKey(GLFW_KEY_W) ||
            window.GetInputKey(GLFW_KEY_S) ||
            window.GetInputKey(GLFW_KEY_A) ||
            window.GetInputKey(GLFW_KEY_D)) {

            cameraTransform->Translate(direction * cameraSpeed * delta_time);
        }

        double x, y;

        window.GetCursorPos(&x, &y);

        directionRot = Vector3(x - directionRot.x, y - directionRot.y, directionRot.z);

        float Yaw = directionRot.x * delta_time * 30;
        float Pitch = directionRot.y * delta_time * 30;

        if (Yaw != 0 || Pitch != 0) {
            cameraTransform = camera->GetComponent<Transform>();
            cameraTransform->RotateYaw(-Yaw);
            cameraTransform->RotatePitch(Pitch);
        }
        directionRot = Vector3(x, y, directionRot.z);
    }
    return 0;
}


// glm::mat4 model = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 1.0f, 0.0f));
// glm::mat4 view = glm::lookAt(cameraPos, cameraAt, cameraUp);
// glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
//
// glm::mat4 mvp = projection * view * model;

// float angle = 0.01f;
// glm::mat4x4 rotX(1.0f);
//
// rotX[1][1] = std::cos(angle);
// rotX[1][2] = -std::sin(angle);
// rotX[2][1] = std::sin(angle);
// rotX[2][2] = std::cos(angle);
//
// glm::mat4x4 rotY(1.0f);
//
// rotY[0][0] = std::cos(angle);
// rotY[0][2] = std::sin(angle);
// rotY[2][0] = -std::sin(angle);
// rotY[2][2] = std::cos(angle);

// matrix[0][0] = 1;
// matrix[1][1] = 1;
// matrix[2][2] = 1;
// matrix[3][3] = 1;
//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
