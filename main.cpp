#include "src/DIContainer.h"
#include "src/FPSCamera.h"
#include "src/Scene.h"
#include "src/Window.h"
#include "src/gameObject/Camera.h"
#include "src/gameObject/primitives/Cube.h"
#include "src/graphics/DefaultMaterial.h"
#include "src/components/MeshRenderer.h"

class MeshRenderer;
class Camera;

int main() {
    const auto* container = new DIContainer();

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

    auto* litShader = container->assetManager->Import<Shader>("basic_lit");
    auto* texture = container->assetManager->Import<Texture>(R"(assets/models/glove/texture/diffuse.jpg)");
    auto* defaultMaterial = new DefaultMaterial(litShader);

    defaultMaterial->color = Color(1.0f, 1.0f, 1.0f);
    defaultMaterial->texture = texture;

    // Building a glove
    Mesh* mesh = container->assetManager->Import<Mesh>(R"(assets/glove.obj)");

    const auto glove = new GameObject();
    glove->name = "glove";

    auto *gloveMeshRenderer = glove->AddComponent<MeshRenderer>();
    gloveMeshRenderer->mesh = mesh;
    gloveMeshRenderer->gameObject = glove;
    gloveMeshRenderer->material = defaultMaterial;

    auto *gloveTransform = glove->GetComponent<Transform>();
    gloveTransform->pivot = mesh->center;

    const auto cube1 = Cube::BuildCube();
    cube1->GetComponent<MeshRenderer>()->material = defaultMaterial;
    const auto cube2 = Cube::BuildCube();
    cube2->GetComponent<MeshRenderer>()->material = defaultMaterial;

    auto* scene = new Scene(camera);
    scene->AddLight(light);
    scene->AddGameObject(cube1);
    scene->AddGameObject(cube2);
    scene->AddGameObject(glove);

    window->AttachScene(scene);

    // scene->Init();

    while (!window->IsClosed()) {
        window->Update();
    }
    return 0;
}
