#include "cmake-build-debug/_deps/assimp-src/code/AssetLib/Blender/BlenderDNA.h"
#include "src/di/DIContainer.h"
#include "src/renderer/domain/Model.h"
#include "src/FPSCamera.h"
#include "src/Scene.h"
#include "src/GLFWWindowImpl.h"
#include "src/InputController.h"
#include "src/components/Behaviour.h"
#include "src/gameObject/Camera.h"
#include "src/gameObject/primitives/Cube.h"
#include "src/components/MeshRenderer.h"
#include "src/components/light/LightFactory.h"
#include "src/components/physics/collision/BoxCollider.h"
#include "src/components/light/PointLight.h"
#include "src/components/light/SpotLight.h"
#include "src/game/RotationComponent.h"
#include "src/IO/FileReader.h"
#include "src/renderer/gl/CommandBuffer.h"

class MeshRenderer;
class Camera;
class BoxCollider;

int main() {
    constexpr int width = 2160;
    constexpr int height = 1440;

    DIContainer *container = &DIContainer::GetInstance();

    Rendering::IRenderer* renderer = container->Get<Rendering::IRenderer>();

    AssetManager* assetManager = container->Get<AssetManager>();

    auto phongShader = assetManager->Import<Shader>("phong/phong");
    auto depthShader = assetManager->Import<Shader>("depth/depth");

    renderer->SetDepthShader(*depthShader);

    auto material1 = new Material(phongShader);

    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
    };

    std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4
    };

    Mesh* mesh = renderer->CreateMesh(vertices, indices);

    auto gameObject = new GameObject("test");
    gameObject->AddComponent<Transform>();
    gameObject->GetComponent<Transform>()->position = Vector3(0, 4.0f, -3.0f);
    gameObject->AddComponent<MeshRenderer>();
    gameObject->GetComponent<MeshRenderer>()->mesh = std::make_shared<Mesh>(*mesh);
  //  gameObject->GetComponent<MeshRenderer>()->material = material1;
    gameObject->AddComponent<Game::RotationComponent>();

    auto *camera = new FPSCamera(60, 0.01, 1000, static_cast<float>(width) / static_cast<float>(height));
    camera->GetTransform()->position = Vector3(10, 0, 10);
    camera->Start();

    auto *houseModel = container->Get<AssetManager>()->Import<Model>(
    R"(assets/models/autumn-house/source/House_scene_01.fbx)");

    const auto cube1 = Cube::BuildCube();
    cube1->GetComponent<MeshRenderer>()->material = material1;
    cube1->GetComponent<Transform>()->position = Vector3(0, 4.0f, -10.0f);

    TextureAsset* diffuseTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(R"(assets/models/glove/texture/wood_with_steel_diffuse.png)");

    Rendering::Texture* diffuseTexture = new Rendering::Texture(diffuseTextureAsset->Width, diffuseTextureAsset->Height, diffuseTextureAsset->Wrap, diffuseTextureAsset->Data);

    TextureAsset* specularTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(R"(assets/models/glove/texture/wood_with_steel_specular.png)");

    Rendering::Texture* specularTexture = new Rendering::Texture(specularTextureAsset->Width, specularTextureAsset->Height, specularTextureAsset->Wrap, specularTextureAsset->Data);

    TextureAsset* houseTextureAsset1 = container->Get<AssetManager>()->Import<TextureAsset>(R"(assets/models/autumn-house/textures/House_texture_01_00.png)");

    Rendering::Texture* houseTexture1 = new Rendering::Texture(houseTextureAsset1->Width, houseTextureAsset1->Height, houseTextureAsset1->Wrap, houseTextureAsset1->Data);

    TextureAsset* houseTextureAsset2 = container->Get<AssetManager>()->Import<TextureAsset>(R"(assets/models/autumn-house/textures/House_texture_02_00.png)");

    Rendering::Texture* houseTexture2 = new Rendering::Texture(houseTextureAsset2->Width, houseTextureAsset2->Height, houseTextureAsset2->Wrap, houseTextureAsset2->Data);

    TextureAsset* carTextureAsset1 = container->Get<AssetManager>()->Import<TextureAsset>(R"(assets/models/autumn-house/textures/Car_textures_01_01.png)");

    Rendering::Texture* carTexture1 = new Rendering::Texture(carTextureAsset1->Width, carTextureAsset1->Height, carTextureAsset1->Wrap, carTextureAsset1->Data);

    TextureAsset* plainTextureAsset1 = container->Get<AssetManager>()->Import<TextureAsset>(R"(assets/models/autumn-house/textures/Plain_textures_01_00.png)");

    Rendering::Texture* plainTexture1 = new Rendering::Texture(plainTextureAsset1->Width, plainTextureAsset1->Height, plainTextureAsset1->Wrap, plainTextureAsset1->Data);

    TextureAsset* plantsTextureAsset1 = container->Get<AssetManager>()->Import<TextureAsset>(R"(assets/models/autumn-house/textures/Plants_textures_01_00.png)");

    Rendering::Texture* plantsTexture1 = new Rendering::Texture(plantsTextureAsset1->Width, plantsTextureAsset1->Height, plantsTextureAsset1->Wrap, plantsTextureAsset1->Data);

    houseModel->materials[0].SetTexture(4, *houseTexture2);
    houseModel->materials[1].SetTexture(4, *houseTexture1);
    houseModel->materials[2].SetTexture(4, *plantsTexture1);
    houseModel->materials[3].SetTexture(4, *carTexture1);
    houseModel->materials[4].SetTexture(4, *plainTexture1);
    houseModel->materials[5].SetTexture(4, *plantsTexture1);

    houseModel->materials[0].SetProperty("material.shininess", 32.0f);
    houseModel->materials[1].SetProperty("material.shininess", 32.0f);
    houseModel->materials[2].SetProperty("material.shininess", 32.0f);
    houseModel->materials[3].SetProperty("material.shininess", 32.0f);
    houseModel->materials[4].SetProperty("material.shininess", 32.0f);
    houseModel->materials[5].SetProperty("material.shininess", 32.0f);

    material1->SetTexture(4, *diffuseTexture);
    material1->SetTexture(5, *specularTexture);
    material1->SetProperty("material.shininess", 32.0f);

    const auto lightFactory = new LightFactory();

    GameObject *ambientLight = lightFactory->SpawnAmbientLight();
    ambientLight->GetComponent<Light>()->color = Color(1.0f, 1.0f, 1.0f, 1.0f);
    ambientLight->GetComponent<Light>()->intensity = 0.01f;

    GameObject *sun = lightFactory->SpawnDirectionalLight();
    sun->GetComponent<Transform>()->position = Vector3(0, 5, 0);
    sun->GetComponent<Transform>()->rotation = Vector3(90, 0, 0);
    sun->GetComponent<Light>()->intensity = 1.0f;

    GameObject *light = lightFactory->SpawnSpotLight();
    light->GetComponent<Transform>()->position = Vector3(0, 6, -10);
    light->GetComponent<Transform>()->rotation = Vector3(90, 0, 0);
    light->GetComponent<SpotLight>()->intensity = 4.0f;
    light->GetComponent<SpotLight>()->SetInnerAngle(3.0f);
    light->GetComponent<SpotLight>()->SetOuterAngle(10.0f);
    light->GetComponent<SpotLight>()->SetRange(10.0f);

    GameObject *light2 = lightFactory->SpawnSpotLight();
    light2->GetComponent<Transform>()->position = Vector3(-1, 6, 10);
    light2->GetComponent<Transform>()->rotation = Vector3(90, 0, 0);
    light2->GetComponent<SpotLight>()->intensity = 4.0f;
    light2->GetComponent<SpotLight>()->SetInnerAngle(3.0f);
    light2->GetComponent<SpotLight>()->SetOuterAngle(10.0f);
    light2->GetComponent<SpotLight>()->SetRange(10.0f);

    GameObject *lamp = lightFactory->SpawnPointLight();
    lamp->GetComponent<Transform>()->position = Vector3(0, 2, 0);
    lamp->GetComponent<Transform>()->rotation = Vector3(0, 0, 0);
    lamp->GetComponent<PointLight>()->intensity = 4.0f;
    lamp->GetComponent<PointLight>()->SetRange(10.0f);

    GameObject *lamp2 = lightFactory->SpawnPointLight();
    lamp2->GetComponent<Transform>()->position = Vector3(-1, 2, 0);
    lamp2->GetComponent<Transform>()->rotation = Vector3(0, 0, 0);
    lamp2->GetComponent<PointLight>()->intensity = 4.0f;
    lamp2->GetComponent<PointLight>()->SetRange(10.0f);

    auto *scene = new Scene(camera);
    scene->Add(cube1);
    scene->Add(gameObject);
    scene->Add(houseModel->root);
    scene->Add(ambientLight);
    scene->Add(sun);
    // scene->Add(light);
    // scene->Add(light2);
    // scene->Add(lamp);
    // scene->Add(lamp2);

    scene->Init();

    double delta_time = 0;

    const auto controller = DIContainer::GetInstance().Get<InputController>();

    float t = 16;

    while (container->Get<IWindow>()->IsOpen()) {
        controller->Update();
        delta_time = container->Get<IWindow>()->GetDeltaTime();
        gameObject->GetComponent<Behaviour>()->Update(delta_time);
        camera->Update(delta_time);
        ambientLight->GetComponent<Light>()->Update(delta_time);
        sun->GetComponent<Light>()->Update(delta_time);
        t += delta_time * 127;
        material1->SetProperty("material.shininess", t);

        if (t >= 256) {
            t = 16;
        }

        renderer->BeginFrame(*camera);
        renderer->Render(*scene);
    }

    return 0;
}

