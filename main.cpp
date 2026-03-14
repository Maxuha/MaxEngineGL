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

    Vector3 Zero() {
        return Vector3(0.0f, 0.0f, 0.0f);
    }

    Vector3 operator-(const Vector3 &v) const {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }

    Vector3 operator+(const Vector3 &v) const {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }

    Vector3 operator*(const float value) const {
        return Vector3(x * value, y * value, z * value);
    }

    Vector3 operator/(const float value) const {
        return Vector3(x / value, y / value, z / value);
    }

    Vector3 operator+=(const Vector3 &v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    Vector3 operator-=(const Vector3 &v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    Vector3 operator*=(const Vector3 &v) {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    }

    Vector3 operator/=(const Vector3 &v) {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        return *this;
    }

    float Length(const Vector3 &v) {
        return sqrt(x * x + y * y + z * z);
    }

    Vector3 Normalize() {
        float length = Length(*this);

        // if length == 0, vector can't be normalized
        if (length == 0.0f) {
            return Zero();
        }

        return Vector3(x / length, y / length, z / length);
    }

    Vector3 CrossProduct(const Vector3 &a, const Vector3 &b) {
        return Vector3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    }

    float DotProduct(const Vector3 &a, const Vector3 &b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
};

class Matrix4x4 {
public:
    Matrix4x4();

    float m[4][4];
};

class Triangle {
public:
    Vector3 vertices[3];
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

    glm::mat4 ViewMatrix() const {
        const glm::vec3 cameraPos = {Position.x, Position.y, Position.z};
        const glm::vec3 cameraAt = {Forward.x, Forward.y, Forward.z};
        const glm::vec3 cameraUp = {Up.x, Up.y, Up.z};

        return glm::lookAt(cameraPos, cameraPos + cameraAt, cameraUp);
    }

    glm::mat4 ViewProjectionMatrix() const {
        return glm::perspective(glm::radians(fov), aspectRatio, near, far) * ViewMatrix();
    }

    void Translate(const Vector3 to, const float speed) {
        const Vector3 right = Vector3().CrossProduct(Up, Forward);

        //Vector3 dir = Vector3().Zero();
        // dir.x = Vector3().DotProduct(to, right);
        // dir.y = Vector3().DotProduct(to, Up);
        // dir.z = Vector3().DotProduct(to, Forward);

        Vector3 dir = right * to.x + Up * to.y + Forward * to.z;

        std::cout << "Right: " << right.x << " " << right.y << " " << right.z << std::endl;
        std::cout << "Up: " << Up.x << " " << Up.y << " " << Up.z << std::endl;
        std::cout << "Forward: " << Forward.x << " " << Forward.y << " " << Forward.z << std::endl;

        std::cout << "global: " << to.x << " " << to.y << " " << to.z << " " << std::endl;
        std::cout << "dir: " << dir.x << " " << dir.y << " " << dir.z << " " << std::endl;

        Position += dir * speed ;
    }

    void RotateY(float Yaw) {
        glm::mat4 matRot = glm::rotate(glm::mat4(1), glm::radians(Yaw), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::vec4 vecRot = matRot * glm::vec4(Forward.x, Forward.y, Forward.z, 0);
        Forward = Vector3(vecRot.x, vecRot.y, vecRot.z).Normalize();
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

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

class MeshRenderer {
public:
    MeshRenderer() {
    }

    Mesh mesh;

    Camera *camera;

    Vector3 position;

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

        glm::mat4 model =
                //glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 1.0f, 0.0f)) *
                glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, position.z));
        glm::mat4 mvp = camera->ViewProjectionMatrix() * model;

        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
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

    static Mesh CreateLine(Vector3 direction) {
        auto box = Mesh();

        box.tris = {
            // SOUTH
                {{{0.0f, 0.0f, 0.0f}, {0.0f, direction.y, 0.0f}, {direction.x, direction.y, 0.0f}}},
                {{{0.0f, 0.0f, 0.0f}, {direction.x, direction.y, 0.0f}, {direction.x, 0.0f, 0.0f}}},

                // EAST
                {{{direction.x, 0.0f, 0.0f}, {direction.x, direction.y, 0.0f}, {direction.x, direction.y, direction.z}}},
                {{{direction.x, 0.0f, 0.0f}, {direction.x, direction.y, direction.z}, {direction.x, 0.0f, direction.z}}},

                // NORTH
                {{{direction.x, 0.0f, direction.z}, {direction.x, direction.y, direction.z}, {0.0f, direction.y, direction.z}}},
                {{{direction.x, 0.0f, direction.z}, {0.0f, direction.y, direction.z}, {0.0f, 0.0f, direction.z}}},

                // WEST
                {{{0.0f, 0.0f, direction.z}, {0.0f, direction.y, direction.z}, {0.0f, direction.y, 0.0f}}},
                {{{0.0f, 0.0f, direction.z}, {0.0f, direction.y, 0.0f}, {0.0f, 0.0f, 0.0f}}},

                // TOP
                {{{0.0f, direction.y, 0.0f}, {0.0f, direction.y, direction.z}, {direction.x, direction.y, direction.z}}},
                {{{0.0f, direction.y, 0.0f}, {direction.x, direction.y, direction.z}, {direction.x, direction.y, 0.0f}}},

                // BOTTOM
                {{{direction.x, 0.0f, direction.z}, {0.0f, 0.0f, direction.z}, {0.0f, 0.0f, 0.0f}}},
                {{{direction.x, 0.0f, direction.z}, {0.0f, 0.0f, 0.0f}, {direction.x, 0.0f, 0.0f}}}
        };

        return box;
    }
};

int main() {
    Window window = Window(2000, 1500);
    window.MakeWindow();

    auto *camera = new Camera();
    camera->Position = {0.0, 1.0, -3.0};
    camera->Forward = Vector3(0.0, 0.0, 1.0);
    camera->Up = {0.0, 1.0, 0.0};
    camera->fov = 45;
    camera->near = 0.01;
    camera->far = 100;
    camera->aspectRatio = static_cast<float>(window.width) / static_cast<float>(window.height);

    //render cube
    auto Box = MeshPrimitives::CreateBox();
    MeshRenderer mesh_renderer;
    mesh_renderer.camera = camera;
    mesh_renderer.mesh = Box;
    mesh_renderer.position = Vector3(-2.0f, 0.0f, 0.0f);

    auto Box2 = MeshPrimitives::CreateBox();
    MeshRenderer mesh_renderer2;
    mesh_renderer2.camera = camera;
    mesh_renderer2.mesh = Box2;
    mesh_renderer2.position = Vector3(2.0f, 0.0f, 0.0f);

    auto Box3 = MeshPrimitives::CreateBox();
    MeshRenderer mesh_renderer7;
    mesh_renderer7.camera = camera;
    mesh_renderer7.mesh = Box3;
    mesh_renderer7.position = Vector3(0.0f, 0.0f, 0.0f);

    auto LineX = MeshPrimitives::CreateLine(Vector3(1,.2,.2));
    MeshRenderer mesh_renderer3;
    mesh_renderer3.camera = camera;
    mesh_renderer3.mesh = LineX;
    mesh_renderer3.position = Vector3(4.0f, 0.0f, 0.0f);

    auto LineY = MeshPrimitives::CreateLine(Vector3(.2,1,.2));
    MeshRenderer mesh_renderer4;
    mesh_renderer4.camera = camera;
    mesh_renderer4.mesh = LineY;
    mesh_renderer4.position = Vector3(4.0f, 0.0f, 0.0f);

    auto LineZ = MeshPrimitives::CreateLine(Vector3(.2,.2,1));
    MeshRenderer mesh_renderer5;
    mesh_renderer5.camera = camera;
    mesh_renderer5.mesh = LineZ;
    mesh_renderer5.position = Vector3(4.0f, 0.0f, 0.0f);

    double last_time = glfwGetTime();
    double delta_time = 0;

    //Warning
    Vector3 directionRotX = Vector3().Zero();

    while (!window.IsClosed()) {
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mesh_renderer.Render();
        mesh_renderer2.Render();
        mesh_renderer3.Render();
        mesh_renderer4.Render();
        mesh_renderer5.Render();
        mesh_renderer7.Render();
        window.SwapBuffers();
        glfwPollEvents();

        delta_time = glfwGetTime() - last_time;
        last_time = glfwGetTime();

        // Camera control
        // Camera translate
        auto direction = Vector3(0.0f, 0.0f, 0.0f);

        if (window.GetInputKey(GLFW_KEY_W)) {
            direction += Vector3(0.0f, 0.0f, 1.0f);
            //camera->Translate(Vector3(0.0f, 0.0f, 1.0f), 2 * delta_time);
        }
        if (window.GetInputKey(GLFW_KEY_S)) {
            direction += Vector3(0.0f, 0.0f, -1.0f);
            //camera->Translate(Vector3(0.0f, 0.0f, -1.0f), 2 * delta_time);
        }
        if (window.GetInputKey(GLFW_KEY_A)) {
            direction += Vector3(1.0f, 0.0f, 0.0f);
            //camera->Translate(Vector3(1.0f, 0.0f, 0.0f), 2 * delta_time);
        }
        if (window.GetInputKey(GLFW_KEY_D)) {
            direction += Vector3(-1.0f, 0.0f, 0.0f);
            //camera->Translate(Vector3(-1.0f, 0.0f, 0.0f), 2 * delta_time);
        }

        if (window.GetInputKey(GLFW_KEY_W) || window.GetInputKey(GLFW_KEY_S) || window.GetInputKey(GLFW_KEY_A) || window
            .GetInputKey(GLFW_KEY_D)) {
            direction = direction.Normalize();
            camera->Translate(direction, 2 * delta_time);
        }

        double x, y;

        window.GetCursorPos(&x, &y);

        directionRotX = Vector3(x - directionRotX.x, directionRotX.y, directionRotX.z).Normalize();

        float Yaw = -directionRotX.x * delta_time * 90;
        if (Yaw != 0) {
            camera->RotateY(Yaw);
        }
        directionRotX = Vector3(x, directionRotX.y, directionRotX.z);

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
