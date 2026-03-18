//
// Created by zykov on 3/15/2026.
//

#include "MeshRenderer.h"
#include <vector>

#include "../gameObject/Camera.h"
#include "glad/glad.h"
#include "../graphics/Triangle.h"
#include <glm/gtc/type_ptr.hpp>

#include "Transform.h"
#include "glm/fwd.hpp"
#include "glm/ext/matrix_transform.hpp"

void MeshRenderer::Init() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
}

void MeshRenderer::Render() {
    std::vector<float> Vertecies;

    for (auto tris: mesh.tries) {
        for (auto vertex: tris.vertices) {
            Vertecies.push_back(vertex.position.x);
            Vertecies.push_back(vertex.position.y);
            Vertecies.push_back(vertex.position.z);

            Vertecies.push_back(vertex.normal.x);
            Vertecies.push_back(vertex.normal.y);
            Vertecies.push_back(vertex.normal.z);
        }
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, Vertecies.size() * sizeof(float), Vertecies.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    GLint mLocation = glGetUniformLocation(shaderProgram, "model");
    GLint color = glGetUniformLocation(shaderProgram, "color");

    Vector3 pivot = gameObject->GetComponent<Transform>()->pivot;
    Vector3 position = gameObject->GetComponent<Transform>()->position;
    Vector3 rotation = gameObject->GetComponent<Transform>()->rotation;
    Vector3 scale = gameObject->GetComponent<Transform>()->scale;

    Vector3 up = gameObject->GetComponent<Transform>()->up;
    Vector3 forward = gameObject->GetComponent<Transform>()->forward;
    Vector3 right = gameObject->GetComponent<Transform>()->right;

    auto model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
    model = glm::translate(model, glm::vec3(pivot.x, pivot.y, pivot.z));
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(right.x, right.y, right.z));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(up.x, up.y, up.z));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(forward.x, forward.y, forward.z));
    model = glm::scale(model, glm::vec3(scale.x, scale.y, scale.z));
    model = glm::translate(model, glm::vec3(-pivot.x, -pivot.y, -pivot.z));

    glUniformMatrix4fv(mLocation, 1, GL_FALSE, glm::value_ptr(model));

    glUniform3f(color, 0.0f, 0.5f, 0.0f);

    glDrawArrays(GL_TRIANGLES, 0, Vertecies.size() / 3);
}

void MeshRenderer::Clear() const {
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderProgram);
}
