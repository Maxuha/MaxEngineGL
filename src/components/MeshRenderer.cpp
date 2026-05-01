//
// Created by zykov on 3/15/2026.
//

#include "MeshRenderer.h"
#include "../gameObject/Camera.h"
#include "../gameObject/light/Light.h"
#include "../graphics/DefaultMaterial.h"

void MeshRenderer::Draw(std::vector<Light*>& lights, Camera& camera) const {
   // material->Enable(lights, camera, *GetGameObject());
    mesh->Bind();

    glDrawElements(GL_TRIANGLES, mesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr);

    mesh->Unbind();
   // material->Disable();
}

void MeshRenderer::Draw2(std::vector<Light*>& lights, Camera& camera) const {
    material->Enable(lights, camera, *GetGameObject());
    // mesh->Bind();
    //
    // glDrawElements(GL_TRIANGLES, mesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
    //
    // mesh->Unbind();
    // material->Disable();
}