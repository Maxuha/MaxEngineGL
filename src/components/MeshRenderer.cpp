//
// Created by zykov on 3/15/2026.
//

#include "MeshRenderer.h"
#include <vector>

#include "Camera.h"
#include "glad/glad.h"
#include "../graphics/Triangle.h"
#include <glm/gtc/type_ptr.hpp>
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

    glm::mat4 model =
            //glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 1.0f, 0.0f)) *
            glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, position.z));
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
