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

    // auto *camera = new FPSCamera(60, 0.01, 1000, static_cast<float>(width) / static_cast<float>(height));
    // camera->GetTransform()->position = Vector3(10, 0, 10);
    //
    // auto *litShader = container->Get<AssetManager>()->Import<Shader>("phong/phong");
    // litShader->AddProperty(ShaderProperty::SHININESS, ShaderMetaProperty(0, sizeof(float)));
    // litShader->AddProperty(ShaderProperty::COLOR_DIFFUSE, ShaderMetaProperty(16, sizeof(Vector4)));
    //
    // const auto* diffuseTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(R"(assets/models/Survival_BackPack_2/1001_albedo.jpg)");
    // const auto* specularTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(R"(assets/models/Survival_BackPack_2/1001_metallic.jpg)");
    //
    // Rendering::IGLTexture* diffuseTexture = new Rendering::GLTexture(diffuseTextureAsset->Width, diffuseTextureAsset->Height, diffuseTextureAsset->Wrap, diffuseTextureAsset->Data);
    // Rendering::IGLTexture* specularTexture = new Rendering::GLTexture(specularTextureAsset->Width, specularTextureAsset->Height, diffuseTextureAsset->Wrap, specularTextureAsset->Data);
    //
    // //
    // // stbi_image_free(diffuseTextureAsset->Data);
    // // stbi_image_free(specularTextureAsset->Data);
    // // delete diffuseTextureAsset;
    // // delete specularTextureAsset;
    // //
    //
    // auto *defaultMaterial = new Material(litShader);
    //
    // container->Get<Rendering::IRenderer>()->MaterialSetTexture(defaultMaterial->GetId(), "diffuse", diffuseTexture->GetId());
    // container->Get<Rendering::IRenderer>()->MaterialSetTexture(defaultMaterial->GetId(), "specular", specularTexture->GetId());
    //
    // defaultMaterial->SetTexture("diffuse", *diffuseTexture);
    // defaultMaterial->SetTexture("specular", *specularTexture);
    // defaultMaterial->SetProperty("material.shininess", 32.0f);
    // defaultMaterial->SetProperty("material.diffuseColor", Color(1.0f, 1.0f, 1.0f, 1.0f));
    //
    // // Building a glove
    // const auto *gloveModel = container->Get<AssetManager>()->Import<Model>(
    //     R"(assets/models/Survival_BackPack_2/Survival_BackPack_2.fbx)");
    //
    // const auto glove = new GameObject();
    //
    // for (auto &entry: gloveModel->meshes) {
    //     const auto gloveTemp = new GameObject();
    //     auto *gloveMeshRenderer = gloveTemp->AddComponent<MeshRenderer>();
    //     gloveMeshRenderer->mesh = entry.mesh;
    //     gloveMeshRenderer->material = defaultMaterial;
    //
    //     auto *gloveTransform = gloveTemp->GetTransform();
    //
    //     gloveTransform->position = entry.position;
    //     gloveTransform->rotation = entry.rotation;
    //     gloveTransform->scale = entry.scale;
    //
    //     gloveTransform->SetParent(glove->GetTransform());
    //     glove->GetTransform()->AddChild(gloveTransform);
    // }
    //
    // glove->GetTransform()->position = Vector3(0, 0, 12);
    // glove->GetTransform()->rotation = Vector3(0, 180, 0);
    // glove->GetTransform()->scale = Vector3(0.005f, 0.005f, 0.005f);
    //
    // //character
    //
    // // const auto *characterModel = container->Get<AssetManager>()->Import<Model>(
    // //   R"(assets/models/Erika.fbx)");
    // //
    // // const auto character = characterModel->root;
    //
    // //house
    //
    // auto *houseModel = container->Get<AssetManager>()->Import<Model>(
    //   R"(assets/models/autumn-house/source/House_scene_01.fbx)");
    //
    // const auto house = houseModel->root;
    //
    // diffuseTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(R"(assets/models/autumn-house/textures/House_texture_01_00.png)");
    // specularTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(R"(assets/models/autumn-house/textures/House_texture_02_00.png)");
    //
    // diffuseTexture = new Rendering::GLTexture(diffuseTextureAsset->Width, diffuseTextureAsset->Height, diffuseTextureAsset->Wrap, diffuseTextureAsset->Data);
    // specularTexture = new Rendering::GLTexture(specularTextureAsset->Width, specularTextureAsset->Height, specularTextureAsset->Wrap, specularTextureAsset->Data);
    //
    // for (auto& material : houseModel->materials) {
    //     material.SetTexture("diffuse", *diffuseTexture);
    //     material.SetTexture("specular", *specularTexture);
    //     material.SetProperty("material.shininess", 32.0f);
    //     material.SetProperty("material.diffuseColor", Color(1.0f, 1.0f, 1.0f, 1.0f));
    //     container->Get<Rendering::IRenderer>()->MaterialSetTexture(material.GetId(), "diffuse", diffuseTexture->GetId());
    //     container->Get<Rendering::IRenderer>()->MaterialSetTexture(material.GetId(), "specular", specularTexture->GetId());
    // }
    //
    // defaultMaterial = new Material(litShader);
    //
    // defaultMaterial->SetTexture("diffuse", *diffuseTexture);
    // defaultMaterial->SetTexture("specular", *specularTexture);
    // defaultMaterial->SetProperty("material.shininess", 32.0f);
    // defaultMaterial->SetProperty("material.diffuseColor", Color(1.0f, 1.0f, 1.0f, 1.0f));
    //
    // house->GetTransform()->position = Vector3(0, -2, 80);
    // house->GetTransform()->rotation = Vector3(0, 180, 0);
    // house->GetTransform()->scale = Vector3(1.0f, 1.0f, 1.0f);
    //
    // // end house
    //
    // //
    // // stbi_image_free(diffuseTextureAsset->Data);
    // // stbi_image_free(specularTextureAsset->Data);
    // // delete diffuseTextureAsset;
    // // delete specularTextureAsset;
    // //
    //
    // diffuseTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(R"(assets/models/glove/texture/wood_with_steel_diffuse.png)");
    // specularTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(R"(assets/models/glove/texture/wood_with_steel_specular.png)");
    //
    // diffuseTexture = new Rendering::GLTexture(diffuseTextureAsset->Width, diffuseTextureAsset->Height, diffuseTextureAsset->Wrap, diffuseTextureAsset->Data);
    // specularTexture = new Rendering::GLTexture(specularTextureAsset->Width, specularTextureAsset->Height, diffuseTextureAsset->Wrap, specularTextureAsset->Data);
    //
    // defaultMaterial = new Material(litShader);
    //
    // container->Get<Rendering::IRenderer>()->MaterialSetTexture(defaultMaterial->GetId(), "diffuse", diffuseTexture->GetId());
    // container->Get<Rendering::IRenderer>()->MaterialSetTexture(defaultMaterial->GetId(), "specular", specularTexture->GetId());
    //
    // defaultMaterial->SetTexture("diffuse", *diffuseTexture);
    // defaultMaterial->SetTexture("specular", *specularTexture);
    // defaultMaterial->SetProperty("material.shininess", 512.0f);
    // defaultMaterial->SetProperty("material.diffuseColor", Color(1.0f, 1.0f, 1.0f, 1.0f));
    //
    // const auto cube1 = Cube::BuildCube();
    // cube1->GetComponent<MeshRenderer>()->material = defaultMaterial;
    // cube1->AddComponent<BoxCollider>();
    // cube1->GetTransform()->position = Vector3(10, 0, 0);
    //
    // const auto cube2 = Cube::BuildCube();
    // //cube2->AddComponent<BoxCollider>();
    // cube2->GetComponent<MeshRenderer>()->material = defaultMaterial;
    // cube2->GetTransform()->position = Vector3(10, 0, -10);
    //
    // const auto cube3 = Cube::BuildCube();
    // cube3->GetComponent<MeshRenderer>()->material = defaultMaterial;
    // cube3->GetTransform()->position = Vector3(10, 0, 4);
    //
    // const auto cube4 = Cube::BuildCube();
    // cube4->GetComponent<MeshRenderer>()->material = defaultMaterial;
    // cube4->GetTransform()->position = Vector3(10, 0, 8);
    //
    // const auto cube5 = Cube::BuildCube();
    // cube5->GetComponent<MeshRenderer>()->material = defaultMaterial;
    // cube5->GetTransform()->position = Vector3(0, -3, 0);
    // cube5->GetTransform()->scale = Vector3(100, 0.1f, 100);
    //
    // const auto lightFactory = new LightFactory();
    // GameObject *ambientLight = lightFactory->SpawnAmbientLight();
    // ambientLight->GetComponent<Light>()->intensity = 0.25f;
    // GameObject *sun = lightFactory->SpawnDirectionalLight();
    // sun->GetComponent<Transform>()->position = Vector3(10, 20, 0);
    // sun->GetComponent<Transform>()->rotation = Vector3(90, 0, 0);
    // sun->GetComponent<Light>()->intensity = 2.0f;
    // GameObject *pointLight = lightFactory->SpawnPointLight();
    // pointLight->GetComponent<Transform>()->position = Vector3(0, 0.0f, 2);
    // pointLight->GetComponent<PointLight>()->intensity = 1.0f;
    // pointLight->GetComponent<PointLight>()->SetRange(3.0f);
    // GameObject *flashLight = lightFactory->SpawnSpotLight();
    // flashLight->GetComponent<Transform>()->position = Vector3(0, 2.0f, 0);
    // flashLight->GetComponent<Transform>()->rotation = Vector3(90, 0, 0);
    // flashLight->GetComponent<SpotLight>()->intensity = 1.0f;
    //
    // auto *scene = new Scene(camera);
    // scene->Add(ambientLight);
    // scene->Add(sun);
    // scene->Add(flashLight);
    // scene->Add(pointLight);
    // scene->Add(cube1);
    // scene->Add(cube2);
    // scene->Add(cube3);
    // scene->Add(cube4);
    // scene->Add(cube5);
    // scene->Add(glove);
    // scene->Add(house);
    // scene->Add(character);

   // container->Get<IWindow>()->AddView(*scene);

    Rendering::IRenderer* renderer = container->Get<Rendering::IRenderer>();

    // const std::vector<char> vSPV = FileReader::ReadFileBytes(
    //      std::string(ASSETS_ROOT) + "/shaders/test/" + "vert" + ".spv");
    // const std::vector<char> fSPV = FileReader::ReadFileBytes(
    //     std::string(ASSETS_ROOT) + "/shaders/test/" + "frag" + ".spv");

    const std::vector<char> vSPV = FileReader::ReadFileBytes(
     std::string(ASSETS_ROOT) + "/shaders/phong/" + "phong-v" + ".spv");
    const std::vector<char> fSPV = FileReader::ReadFileBytes(
        std::string(ASSETS_ROOT) + "/shaders/phong/" + "phong-f" + ".spv");

    auto mainShader = new Shader(vSPV, fSPV);

     auto material1 = new Material(mainShader);
    // auto material2 = new Material(mainShader);

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

    // for (auto components_in_child : houseModel->root->GetComponentsInChildren<MeshRenderer>()) {
    //     components_in_child->material = material2;
    // }

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

    //fake
    // houseModel->materials[0].SetTexture(5, *houseTexture2);
    // houseModel->materials[1].SetTexture(5, *houseTexture1);
    // houseModel->materials[2].SetTexture(5, *plantsTexture1);
    // houseModel->materials[3].SetTexture(5, *carTexture1);
    // houseModel->materials[4].SetTexture(5, *plainTexture1);
    // houseModel->materials[5].SetTexture(5, *plantsTexture1);

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
    ambientLight->GetComponent<Light>()->intensity = 0.04f;

    GameObject *sun = lightFactory->SpawnDirectionalLight();
    sun->GetComponent<Transform>()->position = Vector3(10, 20, 0);
    sun->GetComponent<Transform>()->rotation = Vector3(60, 0, 0);
    sun->GetComponent<Light>()->intensity = 1.0f;

    auto *scene = new Scene(camera);
    scene->Add(cube1);
    scene->Add(gameObject);
    scene->Add(houseModel->root);
    scene->Add(ambientLight);
    scene->Add(sun);

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

        //std::cout << "material.shininess:" << t << std::endl;
        // std::vector<MeshRenderer *> renderers = scene->GetComponents<MeshRenderer>();
        // std::vector<Rendering::DrawCall> drawCalls;
        // drawCalls.reserve(renderers.size());
        // for (auto renderer : renderers) {
        //     if (renderer != nullptr && renderer->mesh != nullptr && renderer->material != nullptr) {
        //         drawCalls.push_back(Rendering::DrawCall { ._mesh = renderer->mesh.get(), .material = renderer->material , .transform = renderer->GetGameObject()->GetTransform()});
        //     }
        // }
        // TextureAsset* diffuseTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(R"(assets/models/glove/texture/wood_with_steel_diffuse.png)");
        //
        // Rendering::Texture* diffuseTexture = new Rendering::Texture(diffuseTextureAsset->Width, diffuseTextureAsset->Height, diffuseTextureAsset->Wrap, diffuseTextureAsset->Data);

        renderer->BeginFrame(*camera);
        renderer->Render(*scene);
       // renderer->Render(drawCalls);
    }

    return 0;
}


//scene add object
// render 
// scene->getRenderableObject
// creating drawcalls
// send them into renderer
