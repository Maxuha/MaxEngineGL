#include <iostream>

#include "src/di/DIContainer.h"
#include "src/FPSCamera.h"
#include "src/physics/PhysicsEngine.h"
#include "src/Scene.h"
#include "src/Window.h"
#include "src/gameObject/Camera.h"
#include "src/gameObject/primitives/Cube.h"
#include "src/graphics/DefaultMaterial.h"
#include "src/components/MeshRenderer.h"
#include "src/components/physics/collision/BoxCollider.h"
#include "src/math/Matrix3x3.h"

class MeshRenderer;
class Camera;
class BoxCollider;


int main() {
    DIContainer* container = &DIContainer::GetInstance();

    constexpr int width = 1280;
    constexpr int height = 720;

    auto* window = new Window(width, height);
    window->Open();

    auto* camera = new FPSCamera();
    camera->fov = 45;
    camera->near = 0.01;
    camera->far = 100;
    camera->aspectRatio = static_cast<float>(width) / static_cast<float>(height);

    auto* light = new DirectionalLight();
    light->direction = Vector3(1.0f, -1.0f, -1.0f);
    light->intensity = 1.5f;
    light->color = Color(1.0f, 1.0f, 1.0f);

    auto* litShader = container->Get<AssetManager>()->Import<Shader>("basic_lit");
    auto* texture = container->Get<AssetManager>()->Import<Texture>(R"(assets/models/glove/texture/diffuse.jpg)");

    auto* defaultMaterial = new DefaultMaterial(litShader);
    defaultMaterial->color = Color(1.0f, 1.0f, 1.0f);
    defaultMaterial->texture = texture;

    // Building a glove
    Mesh* mesh = container->Get<AssetManager>()->Import<Mesh>(R"(assets/glove.obj)");

    const auto glove = new GameObject();
    glove->name = "glove";

    auto *gloveMeshRenderer = glove->AddComponent<MeshRenderer>();
    gloveMeshRenderer->mesh = mesh;
    gloveMeshRenderer->material = defaultMaterial;

    auto *gloveTransform = glove->GetComponent<Transform>();
    gloveTransform->pivot = mesh->center;

    const auto cube1 = Cube::BuildCube();
    cube1->GetComponent<MeshRenderer>()->material = defaultMaterial;
    cube1->AddComponent<BoxCollider>();

    const auto cube2 = Cube::BuildCube();
    cube2->AddComponent<BoxCollider>();
    cube2->GetComponent<MeshRenderer>()->material = defaultMaterial;

    const auto cube3 = Cube::BuildCube();
    cube3->GetComponent<MeshRenderer>()->material = defaultMaterial;

    auto* scene = new Scene(camera);
    scene->Add(light);
    scene->Add(cube1);
    scene->Add(cube2);
    scene->Add(cube3);
    scene->Add(glove);

    cube1->GetComponent<Transform>()->Translate(cube1->GetComponent<Transform>()->Forward() * -20);
    cube2->GetComponent<Transform>()->Translate(cube2->GetComponent<Transform>()->Forward() * 4);
    cube3->GetComponent<Transform>()->Translate(cube3->GetComponent<Transform>()->Forward() * 6);
    glove->GetComponent<Transform>()->Translate(glove->GetComponent<Transform>()->Forward() * 20);

    // cube2->GetComponent<Transform>()->SetParent(cube1->GetComponent<Transform>());
    // cube3->GetComponent<Transform>()->SetParent(cube2->GetComponent<Transform>());
    // cube2->GetComponent<Transform>()->SetParent(nullptr);
    // cube3->GetComponent<Transform>()->SetParent(nullptr);

    window->AttachScene(scene);

    while (!window->IsClosed()) {
        window->Update();
    }

    return 0;
}
