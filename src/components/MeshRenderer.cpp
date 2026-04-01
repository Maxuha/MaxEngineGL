//
// Created by zykov on 3/15/2026.
//

#include "MeshRenderer.h"
#include <vector>
#include "../gameObject/Camera.h"
#include "glad/glad.h"
#include "../graphics/Triangle.h"
#include "Transform.h"
#include "../graphics/RenderContext.h"
#include "../graphics/DefaultMaterial.h"

void MeshRenderer::Init() {
    for (auto tris: mesh.tries) {
        for (auto vertex: tris.vertices) {
            Vertex.push_back(vertex.position.x);
            Vertex.push_back(vertex.position.y);
            Vertex.push_back(vertex.position.z);

            Vertex.push_back(vertex.normal.x);
            Vertex.push_back(vertex.normal.y);
            Vertex.push_back(vertex.normal.z);
        }
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, Vertex.size() * sizeof(float), Vertex.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void MeshRenderer::Render(const RenderContext &context) const {

    material->shader->Use();

    const Matrix4x4 model = gameObject->GetComponent<Transform>()->GetModelMatrix();

    material->shader->SetUniform("view", context.viewMatrix);
    material->shader->SetUniform("projection", context.projectionMatrix);
    material->shader->SetUniform("model", model);

    material->shader->SetUniform("lightDirection", context.lightDirection);
    material->shader->SetUniform("lightColor", context.lightColor);
    material->shader->SetUniform("lightIntensity", context.lightIntensity);

    material->Apply();

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, Vertex.size() / 6);
    glBindVertexArray(0);
}
