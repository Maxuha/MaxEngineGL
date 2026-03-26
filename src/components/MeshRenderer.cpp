//
// Created by zykov on 3/15/2026.
//

#include "MeshRenderer.h"

#include <iostream>
#include <vector>
#include "../gameObject/Camera.h"
#include "glad/glad.h"
#include "../graphics/Triangle.h"
#include <glm/gtc/type_ptr.hpp>
#include "Transform.h"
#include "../graphics/RenderContext.h"
#include "../graphics/SimpleMaterial.h"
#include "glm/fwd.hpp"
#include "glm/ext/matrix_transform.hpp"

void MeshRenderer::Init() {
    const auto shader = new Shader();

    shader->LoadShader(
        (std::string(SHADERS_ROOT) + "/assets/shaders/basic_lit.vert").c_str(),
        (std::string(SHADERS_ROOT) + "/assets/shaders/basic_lit.frag  ").c_str()
        );

    material = new SimpleMaterial(shader);

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

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, Vertecies.size() * sizeof(float), Vertecies.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void MeshRenderer::Render(RenderContext context) const {

    glUseProgram(material->shaderCompiled);

    const GLint mLocation = glGetUniformLocation(material->shaderCompiled, "model");

    Vector3 pivot = gameObject->GetComponent<Transform>()->pivot;
    Vector3 position = gameObject->GetComponent<Transform>()->position;
    Vector3 rotation = gameObject->GetComponent<Transform>()->rotation;
    Vector3 scale = gameObject->GetComponent<Transform>()->scale;

    Vector3 up = gameObject->GetComponent<Transform>()->Up();
    Vector3 forward = gameObject->GetComponent<Transform>()->Forward();
    Vector3 right = gameObject->GetComponent<Transform>()->Right();

    auto model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
    // model = glm::translate(model, glm::vec3(pivot.x, pivot.y, pivot.z));
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(right.x, right.y, right.z));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(up.x, up.y, up.z));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(forward.x, forward.y, forward.z));
    // model = glm::scale(model, glm::vec3(scale.x, scale.y, scale.z));
    // model = glm::translate(model, glm::vec3(-pivot.x, -pivot.y, -pivot.z));

    material->color = Color(0.0f, 0.5f, 0.0f);

    material->shader->SetMat4("view", context.viewMatrix);
    material->shader->SetMat4("projection", context.projectionMatrix);
    material->shader->SetVec3("lightDirection", context.lightDirection);
    material->shader->SetVec3("lightColor", context.lightColor);
    material->shader->SetFloat("lightIntensity", context.lightIntensity);

    material->Apply();

    glUniformMatrix4fv(mLocation, 1, GL_FALSE, glm::value_ptr(model));

    glDrawArrays(GL_TRIANGLES, 0, Vertecies.size() / 3);
}

void MeshRenderer::Clear() const {
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(material->shaderCompiled);
}
