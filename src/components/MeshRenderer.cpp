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

void MeshRenderer::Render(const RenderContext &context) const {
    const GLint mLocation = glGetUniformLocation(material->shaderCompiled, "model");

    glm::mat4 model = gameObject->GetComponent<Transform>()->model;

    material->color = Color(0.0f, 0.5f, 0.0f);

    material->shader->Use();

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
