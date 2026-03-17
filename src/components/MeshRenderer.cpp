//
// Created by zykov on 3/15/2026.
//

#include "MeshRenderer.h"
#include <vector>

#include "Camera.h"
#include "glad/glad.h"
#include "../graphics/Triangle.h"
#include <glm/gtc/type_ptr.hpp>

#include "Transform.h"
#include "glm/fwd.hpp"
#include "glm/ext/matrix_transform.hpp"

GLuint createShaderProgram() {
    const char *vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;

        uniform mat4 uMVP;

        void main()
        {
            gl_Position = uMVP * vec4(aPos, 1.0);
        }
    )";

    const char *fragmentShaderSource = R"(
        #version 330 core

        out vec4 FragColor;

        uniform vec3 color;

        void main()
        {
            FragColor = vec4(color, 1.0);
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

void MeshRenderer::Render() {
    std::vector<float> Vertecies;

    for (auto tris: mesh.tries) {
        for (auto &[x, y, z]: tris.vertices) {
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
    GLint color = glGetUniformLocation(shaderProgram, "color");

    Vector3 pivot = gameObject->GetComponent<Transform>()->pivot;
    Vector3 position = gameObject->GetComponent<Transform>()->position;
    Vector3 rotation = gameObject->GetComponent<Transform>()->rotation;
    Vector3 scale = gameObject->GetComponent<Transform>()->scale;

    Vector3 up = gameObject->GetComponent<Transform>()->up;
    Vector3 forward = gameObject->GetComponent<Transform>()->forward;
    Vector3 right = gameObject->GetComponent<Transform>()->Right();

    // std::cout << "Position: " << position.x << " " << position.y << " " << position.z << " " << std::endl;
    // std::cout << "Up: " << up.x << " " << up.y << " " << up.z << " " << std::endl;
    // std::cout << "Forward: " << forward.x << " " << forward.y << " " << forward.z << " " << std::endl;
    // std::cout << "Right: " << right.x << " " << right.y << " " << right.z << " " << std::endl;

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
    model = glm::translate(model, glm::vec3(pivot.x, pivot.y, pivot.z));
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(right.x, right.y, right.z));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(up.x, up.y, up.z));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(forward.x, forward.y, forward.z));
    model = glm::scale(model, glm::vec3(scale.x, scale.y, scale.z));
    model = glm::translate(model, glm::vec3(-pivot.x, -pivot.y, -pivot.z));

    glm::mat4 mvp = camera->ViewProjectionMatrix() * model;

    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform3f(color, 0.0f, 0.5f, 0.0f);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void MeshRenderer::Clear() const {
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderProgram);
}
