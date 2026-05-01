//
// Created by zykov on 3/17/2026.
//

#include "Scene.h"
#include "components/MeshRenderer.h"
#include "di/DIContainer.h"
#include "gameObject/light/DirectionalLight.h"
#include "gameObject/primitives/Cube.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "IO/ObjImporter.h"
#include "math/AABB.h"

class AssetManager;

void Scene::Init() {
    for (GameObject *obj: gameObjects) {
        obj->Start();
    }
    depthShader = DIContainer::GetInstance().Get<AssetManager>()->Import<Shader>("depth/depth");
    debugDepthQuad = DIContainer::GetInstance().Get<AssetManager>()->Import<Shader>("depth/depth_debug");

    glEnable(GL_DEPTH_TEST);

    const unsigned int SHADOW_WIDTH = 8192, SHADOW_HEIGHT = 8192;

    glGenFramebuffers(1, &depthMapFBO);
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    debugDepthQuad->Enable();
    debugDepthQuad->SetUniform("depthMap", 0);
}

void renderQuad()
{
    unsigned int quadVAO = 0;
    unsigned int quadVBO;
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), static_cast<void *>(0));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void Scene::Update(const double delta_time) {
      gameObjects[0]->GetComponent<Transform>()->Translate(gameObjects[0]->GetComponent<Transform>()->Forward() * 2.0f * delta_time);
    //  gameObjects[3]->GetComponent<Transform>()->RotateYaw(30.0f * delta_time);
     // gameObjects[1]->GetComponent<Transform>()->RotateYaw(-50.0f * delta_time);
     // gameObjects[2]->GetComponent<Transform>()->RotateYaw(30.0f * delta_time);

    camera->Update(delta_time);

    const unsigned int SHADOW_WIDTH = 8192, SHADOW_HEIGHT = 8192;

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    depthShader->Enable();
    depthShader->SetUniform("view", lights[1]->GetTransform()->LookAt());
   // depthShader->SetUniform("projection", lights[1]->GetTransform()->Perspective(45, static_cast<float>(SHADOW_WIDTH) / static_cast<float>(SHADOW_HEIGHT), 0.01, 100));
    depthShader->SetUniform("projection", lights[1]->GetTransform()->Othographic(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 100.0f));

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);

    for (GameObject *obj: gameObjects) {

        obj->Update(delta_time);

        std::vector<MeshRenderer*> renderers;

        GetMeshRenderer(obj->GetTransform(), renderers);

        for (const auto& renderer: renderers) {
            if (renderer == nullptr) continue;
            depthShader->SetUniform("model", renderer->GetGameObject()->GetTransform()->GetWorldMatrix());
            renderer->Draw(lights, *camera);
        }
    }

    glCullFace(GL_BACK);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, 1920, 1080);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // debugDepthQuad->Enable();
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, depthMap);
    // renderQuad();
    //
    // return;

    //lights[1]->GetTransform()->RotatePitch(10 * delta_time);


    for (GameObject *obj: gameObjects) {

        obj->Update(delta_time);

        std::vector<MeshRenderer*> renderers;

        GetMeshRenderer(obj->GetTransform(), renderers);

        for (const auto& renderer: renderers) {
            if (renderer == nullptr) continue;
            renderer->Draw2(lights, *camera);
            dynamic_cast<DefaultMaterial*>(renderer->material)->SetTexture(2);
            // dynamic_cast<DefaultMaterial*>(renderer->material)->SetFloat("near_plane", 0.01);
            dynamic_cast<DefaultMaterial*>(renderer->material)->SetFloat("lightSpaceMatrix",
           lights[1]->GetTransform()->Othographic(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 100.0f) * lights[1]->GetTransform()->LookAt()
             //  lights[1]->GetTransform()->LookAt() *  lights[1]->GetTransform()->Perspective(45, static_cast<float>(SHADOW_WIDTH) / static_cast<float>(SHADOW_HEIGHT), 0.01, 100)
                );

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            renderer->Draw(lights, *camera);
        }
    }

}

void Scene::Add(Light *light) {
    lights.push_back(light);
}

void Scene::Add(GameObject *gameObject) {
    gameObjects.push_back(gameObject);
}

Transform* Scene::GetMeshRenderer(Transform *transform, std::vector<MeshRenderer*>& meshRenderers) {
    meshRenderers.push_back(transform->GetGameObject()->GetComponent<MeshRenderer>());

    for (const auto& pair: transform->children) {
        meshRenderers.push_back(pair->GetGameObject()->GetComponent<MeshRenderer>());
        GetMeshRenderer(pair, meshRenderers);
    }
    return transform;
}
