//
// Created by zykov on 3/15/2026.
//

#include "MeshRenderer.h"
#include "../gameObject/Camera.h"
#include "../graphics/DefaultMaterial.h"

void MeshRenderer::Draw(const RenderContext &context) const {
    material->Enable(context, GetGameObject());
    mesh->Bind();

    glDrawElements(GL_TRIANGLES, mesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr);

    mesh->Unbind();
    material->Disable();
}
