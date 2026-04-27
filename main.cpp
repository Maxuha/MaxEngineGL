#include "cmake-build-debug/_deps/assimp-src/code/AssetLib/Blender/BlenderDNA.h"
#include "src/di/DIContainer.h"
#include "src/graphics/Model.h"
#include "src/FPSCamera.h"
#include "src/Scene.h"
#include "src/Window.h"
#include "src/gameObject/Camera.h"
#include "src/gameObject/primitives/Cube.h"
#include "src/graphics/DefaultMaterial.h"
#include "src/components/MeshRenderer.h"
#include "src/components/physics/collision/BoxCollider.h"
#include "src/gameObject/light/AmbientLight.h"
#include "src/gameObject/light/DirectionalLight.h"
#include "src/gameObject/light/PointLight.h"
#include "src/gameObject/light/SpotLight.h"
#include "src/math/Color.h"

class MeshRenderer;
class Camera;
class BoxCollider;

int main() {
    DIContainer *container = &DIContainer::GetInstance();

    constexpr int width = 1920;
    constexpr int height = 1080;

    auto *window = new Window(width, height);
    window->Open();

    auto *camera = new FPSCamera(45, 0.01, 10000, static_cast<float>(width) / static_cast<float>(height));

    auto *ambientLight = new AmbientLight(Color(1.0f, 1.0f, 1.0f, 1.0f), 0.1f);

    auto *sun = new DirectionalLight(Color(1.0f, 1.0f, 1.0f, 1.0f), 1.0f);
    sun->GetComponent<Transform>()->position = Vector3(0, 5, 0);
    sun->GetComponent<Transform>()->rotation = Vector3(90, 0, 0);

    auto *lamp = new PointLight(Color(1.0f, 1.0f, 1.0f, 1.0f), 5.0f, 10.0f);
    lamp->GetComponent<Transform>()->position = Vector3(0, 4, 9);
    lamp->GetComponent<Transform>()->rotation = Vector3(90, 0, 0);

    auto *flashLight = new SpotLight(Color(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 10.0f, std::cos(glm::radians(12.5f)),
                                     std::cos(glm::radians(17.5f)));

    auto *litShader = container->Get<AssetManager>()->Import<Shader>("phong/phong");

    // auto *diffuseTexture = container->Get<AssetManager>()->Import<
    //     Texture>(R"(assets/models/glove/texture/diffuse.jpg)");
    // auto *specularTexture = container->Get<AssetManager>()->Import<Texture>(
    //     R"(assets/models/glove/texture/diffuse.jpg)");
    //
    auto *diffuseTexture = container->Get<AssetManager>()->Import<
        Texture>(R"(assets/models/Survival_BackPack_2/1001_albedo.jpg)");
    auto *specularTexture = container->Get<AssetManager>()->Import<Texture>(
        R"(assets/models/Survival_BackPack_2/1001_albedo.jpg)");

    auto *defaultMaterial = new DefaultMaterial(litShader, diffuseTexture, specularTexture,
                                                Color(1.0f, 1.0f, 1.0f, 1.0f));

    // Building a glove
    const auto *gloveModel = container->Get<AssetManager>()->Import<Model>(
        R"(assets/models/Survival_BackPack_2/Survival_BackPack_2.fbx)");

    //const auto* gloveModel = container->Get<AssetManager>()->Import<Model>(R"(assets/glove.obj)");

    std::vector<GameObject *> gloves = {};

    const auto glove = new GameObject();

    for (auto &entry: gloveModel->meshes) {
        const auto gloveTemp = new GameObject();
        auto *gloveMeshRenderer = gloveTemp->AddComponent<MeshRenderer>();
        gloveMeshRenderer->mesh = entry.mesh;
        gloveMeshRenderer->material = defaultMaterial;
        gloves.push_back(gloveTemp);

        auto *gloveTransform = gloveTemp->GetTransform();

        gloveTransform->position = entry.position;
        gloveTransform->rotation = entry.rotation;
        gloveTransform->scale = entry.scale;

        gloveTransform->SetParent(glove->GetTransform());
        glove->GetTransform()->AddChild(gloveTransform);
    }

    glove->GetTransform()->position = Vector3(0, 0, 10);
    glove->GetTransform()->rotation = Vector3(0, 180, 0);
    glove->GetTransform()->scale = Vector3(0.005f, 0.005f, 0.005f);

    // auto *gloveMeshRenderer = glove->AddComponent<MeshRenderer>();
    // gloveMeshRenderer->meshes = gloveModel->meshes;
    // gloveMeshRenderer->material = defaultMaterial;

    // auto *gloveTransform = glove->GetComponent<Transform>();
    // gloveTransform->pivot = gloveModel->meshes.at(0)->center;

    diffuseTexture = container->Get<AssetManager>()->Import<Texture>(
        R"(assets/models/glove/texture/wood_with_steel_diffuse.png)");
    specularTexture = container->Get<AssetManager>()->Import<Texture>(
        R"(assets/models/glove/texture/wood_with_steel_specular.png)");

    defaultMaterial = new DefaultMaterial(litShader, diffuseTexture, specularTexture, Color(1.0f, 1.0f, 1.0f, 1.0f));

    const auto cube1 = Cube::BuildCube();
    cube1->GetComponent<MeshRenderer>()->material = defaultMaterial;
    cube1->AddComponent<BoxCollider>();

    const auto cube2 = Cube::BuildCube();
    cube2->AddComponent<BoxCollider>();
    cube2->GetComponent<MeshRenderer>()->material = defaultMaterial;

    const auto cube3 = Cube::BuildCube();
    cube3->GetComponent<MeshRenderer>()->material = defaultMaterial;

    auto *scene = new Scene(camera);
    scene->Add(lamp);
    scene->Add(sun);
    scene->Add(flashLight);
    scene->Add(ambientLight);
    scene->Add(cube1);
    scene->Add(cube2);
    scene->Add(cube3);
    scene->Add(glove);

    // for (GameObject* glove: gloves) {
    //     scene->Add(glove);
    // }

    // glove->GetTransform()->Translate(glove->GetTransform()->Forward() * 500);
    cube1->GetComponent<Transform>()->Translate(cube1->GetComponent<Transform>()->Forward() * -20);
    cube2->GetComponent<Transform>()->Translate(cube2->GetComponent<Transform>()->Forward() * 4);
    cube3->GetComponent<Transform>()->Translate(cube3->GetComponent<Transform>()->Forward() * 6);
    // glove->GetComponent<Transform>()->Translate(glove->GetComponent<Transform>()->Forward() * 20);

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
