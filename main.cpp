#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


GLuint createShaderProgram() {
    const char *vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;

        out vec3 vColor;

        uniform mat4 uMVP;

        void main()
        {
            gl_Position = uMVP * vec4(aPos, 1.0);
            vColor = aColor;
        }
    )";

    const char *fragmentShaderSource = R"(
        #version 330 core
        in vec3 vColor;
        out vec4 FragColor;

        void main()
        {
            FragColor = vec4(vColor, 1.0);
        }
    )";

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

class Vector3 {
public:
    float x, y, z;
};

class Matrix4x4 {
public:
    Matrix4x4();

    float m[4][4];
};

class Vertex {
public:
    float x, y, z;
};

class Triangle {
public:
    Vertex vertices[3];
};

class Mesh {
public:
    std::vector<Triangle> tris;
};

class Camera {
public:
    float fov;
    float near;
    float far;
    float aspectRatio;

    Vector3 Position;
    Vector3 Forward;
    Vector3 Up;

    glm::mat4x4 ViewMatrix() const {
        const glm::vec3 cameraPos = {Position.x, Position.y, Position.z};
        const glm::vec3 cameraAt = {Forward.x, Forward.y, Forward.z};
        const glm::vec3 cameraUp = {Up.x, Up.y, Up.z};

        const glm::mat4 view = glm::lookAt(cameraPos, cameraAt, cameraUp);
        return glm::perspective(fov, aspectRatio, near, far) * view;
    }
};

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

    int IsClosed() const {
        return glfwWindowShouldClose(window);
    }
};

class MeshRenderer {
public:
    MeshRenderer() {
    }

    Mesh mesh;

    Camera *camera;

    unsigned int VBO, VAO;

    GLuint shaderProgram;

    void Render() {
        std::vector<float> Vertecies;

        for (auto &[vertices]: mesh.tris) {
            for (auto &[x, y, z]: vertices) {
                Vertecies.push_back(x);
                Vertecies.push_back(y);
                Vertecies.push_back(z);
            }
        }

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, Vertecies.size() * sizeof(float), Vertecies.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void *>(nullptr));
        glEnableVertexAttribArray(0);

        shaderProgram = createShaderProgram();

        glUseProgram(shaderProgram);

        GLint mvpLocation = glGetUniformLocation(shaderProgram, "uMVP");

        // TODO DELETE
        glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -3.0f);
        glm::vec3 cameraAt = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

        glm::mat4 model = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 1.0f, 0.0f));
        glm::mat4 view = glm::lookAt(cameraPos, cameraAt, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

        glm::mat4 mvp = camera->ViewMatrix() * model;

        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    void Clear() {
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
        glDeleteProgram(shaderProgram);
    }
};

class MeshPrimitives {
public:
    MeshPrimitives() = delete;

    static Mesh CreateBox() {
        auto box = Mesh();

        box.tris = {
            // SOUTH
            {{{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}}},
            {{{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}}},

            // EAST
            {{{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}}},
            {{{1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}}},

            // NORTH
            {{{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}}},
            {{{1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}},

            // WEST
            {{{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}}},
            {{{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}},

            // TOP
            {{{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}}},
            {{{0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}}},

            // BOTTOM
            {{{1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}},
            {{{1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}}}
        };

        return box;
    }
};

int main() {
    auto *window = new Window(800, 600);
    window->MakeWindow();

    auto *camera = new Camera();
    camera->Position = {0.0, 0.0, -3.0};
    camera->Forward = {0.0, 0.0, 1.0};
    camera->Up = {0.0, 1.0, 0.0};
    camera->fov = 45;
    camera->near = 0.01;
    camera->far = 100;
    camera->aspectRatio = 800 / 600;

    //render cube
    auto Box = MeshPrimitives::CreateBox();
    MeshRenderer mesh_renderer;
    mesh_renderer.camera = camera;
    mesh_renderer.mesh = Box;

    while (!window->IsClosed()) {
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mesh_renderer.Render();
        window->SwapBuffers();
        glfwPollEvents();
    }

    // while (!glfwWindowShouldClose(window)) {
    //     auto deltaTime = static_cast<float>(glfwGetTime());
    //
    //     if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    //         cameraPos += cameraAt * deltaTime * cameraSpeed;
    //     }
    //
    //
    //
    //
    // }
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
