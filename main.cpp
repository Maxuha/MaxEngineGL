#include "cmake-build-debug/_deps/assimp-src/code/AssetLib/Blender/BlenderDNA.h"
#include "src/di/DIContainer.h"
#include "src/renderer/domain/Model.h"
#include "src/FPSCamera.h"
#include "src/Scene.h"
#include "src/GLFWWindowImpl.h"
#include "src/InputController.h"
#include "src/gameObject/Camera.h"
#include "src/gameObject/primitives/Cube.h"
#include "src/components/MeshRenderer.h"
#include "src/components/light/LightFactory.h"
#include "src/components/physics/collision/BoxCollider.h"
#include "src/components/light/PointLight.h"
#include "src/components/light/SpotLight.h"
#include "src/gameObject/primitives/Plane.h"
#include "src/IO/FileReader.h"

class MeshRenderer;
class Camera;
class BoxCollider;

int main() {
    constexpr int width = 2160;
    constexpr int height = 1440;

    DIContainer *container = &DIContainer::GetInstance();

    Rendering::IRenderer* renderer = container->Get<Rendering::IRenderer>();

    AssetManager* assetManager = container->Get<AssetManager>();

    auto phongShader = assetManager->Import<Shader>("phong/phong", {{"renderFace", "Both"}});
    auto depthShader = assetManager->Import<Shader>("depth/depth", {{"renderFace", "Front"}, {"isDepthOnly", "true"}});

    renderer->SetDepthShader(*depthShader);

    auto material1 = new Material(phongShader);
    auto material2 = new Material(phongShader);
    auto material3 = new Material(phongShader);

    auto *camera = new FPSCamera(60, 0.01, 1000, static_cast<float>(width) / static_cast<float>(height));
    camera->GetTransform()->position = Vector3(10, 0, 10);
    camera->Start();

    const auto cube1 = Cube::BuildCube();
    cube1->GetComponent<MeshRenderer>()->material = material1;
    cube1->GetComponent<Transform>()->position = Vector3(0, 4.0f, -10.0f);
    cube1->GetComponent<Transform>()->scale = Vector3(1.0f, 1.0f, 1.0f);

    const auto brickwall = Plane::BuildPlane();
    brickwall->GetComponent<MeshRenderer>()->material = material2;
    brickwall->GetComponent<Transform>()->position = Vector3(20, 7.0f, 0.0f);
    brickwall->GetComponent<Transform>()->rotation = Vector3(90, 90.0f, 0.0f);
    brickwall->GetComponent<Transform>()->scale = Vector3(5.0f, 5.0f, 1.0f);

    auto *houseModel = container->Get<AssetManager>()->Import<Model>(
        R"(assets/models/autumn-house/source/House_scene_01.fbx)");
    houseModel->root->GetTransform()->position = Vector3(0, 0, 0);

    auto *backpackModel = container->Get<AssetManager>()->Import<Model>(
        R"(assets/models/Survival_BackPack_2/Survival_BackPack_2.fbx)");

    uint32_t size = backpackModel->root->GetComponentsInChildren<MeshRenderer>().size();

    for (int i = 0; i < size; i++) {
        backpackModel->root->GetComponentsInChildren<MeshRenderer>()[i]->material = material3;
    }

    backpackModel->root->GetTransform()->position = Vector3(5, 5, 4);
    backpackModel->root->GetTransform()->scale = Vector3(0.01, 0.01, 0.01);

    auto *erika = container->Get<AssetManager>()->Import<Model>(R"(assets/models/Erika.fbx)");
    erika->root->GetComponentsInChildren<MeshRenderer>()[0]->material = material1;
    erika->root->GetTransform()->scale = Vector3(0.05, 0.05, 0.05);
    erika->root->GetTransform()->position = Vector3(10, 2, 0);

    TextureAsset *woodDiffuseTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(
      R"(assets/models/brickwall/wood_diffuse.png)");

    Rendering::Texture *woodDiffuseTexture = new Rendering::Texture(woodDiffuseTextureAsset->Width, woodDiffuseTextureAsset->Height,
                                                                 woodDiffuseTextureAsset->Wrap, woodDiffuseTextureAsset->Data);

    TextureAsset *toyBoxDispTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(
      R"(assets/models/brickwall/toy_box_disp.png)");

    Rendering::Texture *toyBoxDispTexture = new Rendering::Texture(toyBoxDispTextureAsset->Width, toyBoxDispTextureAsset->Height,
                                                                toyBoxDispTextureAsset->Wrap, toyBoxDispTextureAsset->Data, Rendering::TextureFormat::RGBA);

    TextureAsset *toyBoxNormalTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(
    R"(assets/models/brickwall/toy_box_normal.png)");

    Rendering::Texture *toyBoxNormalTexture = new Rendering::Texture(toyBoxNormalTextureAsset->Width, toyBoxNormalTextureAsset->Height,
                                                                toyBoxNormalTextureAsset->Wrap, toyBoxNormalTextureAsset->Data, Rendering::TextureFormat::RGBA);

    TextureAsset *backpackDiffuseTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(
      R"(assets/models/Survival_BackPack_2/1001_albedo.jpg)");

    Rendering::Texture *backpackDiffuseTexture = new Rendering::Texture(backpackDiffuseTextureAsset->Width, backpackDiffuseTextureAsset->Height,
                                                                backpackDiffuseTextureAsset->Wrap, backpackDiffuseTextureAsset->Data);

    TextureAsset *backpackSpecularTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(
   R"(assets/models/Survival_BackPack_2/1001_metallic.jpg)");

    Rendering::Texture *backpackSpecularTexture = new Rendering::Texture(backpackSpecularTextureAsset->Width, backpackSpecularTextureAsset->Height,
                                                                backpackSpecularTextureAsset->Wrap, backpackSpecularTextureAsset->Data, Rendering::TextureFormat::RGBA);

    TextureAsset *backpackNormalTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(
 R"(assets/models/Survival_BackPack_2/1001_normal.png)");

    Rendering::Texture *backpackNormalTexture = new Rendering::Texture(backpackNormalTextureAsset->Width, backpackNormalTextureAsset->Height,
                                                                backpackNormalTextureAsset->Wrap, backpackNormalTextureAsset->Data, Rendering::TextureFormat::RGBA);

    TextureAsset *brickwallDiffuseTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(
       R"(assets/models/brickwall/brickwall_diffuse.jpg)");

    Rendering::Texture *brickwallDiffuseTexture = new Rendering::Texture(brickwallDiffuseTextureAsset->Width, brickwallDiffuseTextureAsset->Height,
                                                                brickwallDiffuseTextureAsset->Wrap, brickwallDiffuseTextureAsset->Data);

    TextureAsset *brickwallNormalTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(
      R"(assets/models/brickwall/brickwall_normal.jpg)");

    Rendering::Texture *brickwallNormalTexture = new Rendering::Texture(brickwallNormalTextureAsset->Width, brickwallNormalTextureAsset->Height,
                                                                brickwallNormalTextureAsset->Wrap, brickwallNormalTextureAsset->Data, Rendering::TextureFormat::RGBA);

    TextureAsset *brickwallHeightTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(
     R"(assets/models/brickwall/brickwall_height.jpg)");

    Rendering::Texture *brickwallHeightTexture = new Rendering::Texture(brickwallHeightTextureAsset->Width, brickwallHeightTextureAsset->Height,
                                                                brickwallHeightTextureAsset->Wrap, brickwallHeightTextureAsset->Data, Rendering::TextureFormat::RGBA);

    TextureAsset *defaultNormalTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(
     R"(assets/models/brickwall/default_normal.jpg)");

    Rendering::Texture *defaultNormalTexture = new Rendering::Texture(defaultNormalTextureAsset->Width, defaultNormalTextureAsset->Height,
                                                                defaultNormalTextureAsset->Wrap, defaultNormalTextureAsset->Data, Rendering::TextureFormat::RGBA);

    TextureAsset *diffuseTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(
        R"(assets/models/glove/texture/wood_with_steel_diffuse.png)");

    Rendering::Texture *diffuseTexture = new Rendering::Texture(diffuseTextureAsset->Width, diffuseTextureAsset->Height,
                                                                diffuseTextureAsset->Wrap, diffuseTextureAsset->Data);

    TextureAsset *normalTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(
        R"(assets/models/glove/texture/wood_with_steel_normal.jpg)");

    Rendering::Texture *normalTexture = new Rendering::Texture(normalTextureAsset->Width, normalTextureAsset->Height,
                                                                normalTextureAsset->Wrap, normalTextureAsset->Data, Rendering::TextureFormat::RGBA);

    TextureAsset *specularTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(
        R"(assets/models/glove/texture/wood_with_steel_specular.png)");

    Rendering::Texture *specularTexture = new Rendering::Texture(specularTextureAsset->Width,
                                                                 specularTextureAsset->Height,
                                                                 specularTextureAsset->Wrap,
                                                                 specularTextureAsset->Data,
                                                                 Rendering::TextureFormat::RGBA);

    TextureAsset *houseTextureAsset1 = container->Get<AssetManager>()->Import<TextureAsset>(
        R"(assets/models/autumn-house/textures/House_texture_01_00.png)");

    Rendering::Texture *houseTexture1 = new Rendering::Texture(houseTextureAsset1->Width, houseTextureAsset1->Height,
                                                               houseTextureAsset1->Wrap, houseTextureAsset1->Data);

    TextureAsset *houseTextureAsset2 = container->Get<AssetManager>()->Import<TextureAsset>(
        R"(assets/models/autumn-house/textures/House_texture_02_00.png)");

    Rendering::Texture *houseTexture2 = new Rendering::Texture(houseTextureAsset2->Width, houseTextureAsset2->Height,
                                                               houseTextureAsset2->Wrap, houseTextureAsset2->Data);

    TextureAsset *carTextureAsset1 = container->Get<AssetManager>()->Import<TextureAsset>(
        R"(assets/models/autumn-house/textures/Car_textures_01_01.png)");

    Rendering::Texture *carTexture1 = new Rendering::Texture(carTextureAsset1->Width, carTextureAsset1->Height,
                                                             carTextureAsset1->Wrap, carTextureAsset1->Data);

    TextureAsset *plainTextureAsset1 = container->Get<AssetManager>()->Import<TextureAsset>(
        R"(assets/models/autumn-house/textures/Plain_textures_01_00.png)");

    Rendering::Texture *plainTexture1 = new Rendering::Texture(plainTextureAsset1->Width, plainTextureAsset1->Height,
                                                               plainTextureAsset1->Wrap, plainTextureAsset1->Data);

    TextureAsset *plantsTextureAsset1 = container->Get<AssetManager>()->Import<TextureAsset>(
        R"(assets/models/autumn-house/textures/Plants_textures_01_00.png)");

    Rendering::Texture *plantsTexture1 = new Rendering::Texture(plantsTextureAsset1->Width, plantsTextureAsset1->Height,
                                                                plantsTextureAsset1->Wrap, plantsTextureAsset1->Data);

    houseModel->materials[0].SetTexture(4, *houseTexture2);
    houseModel->materials[1].SetTexture(4, *houseTexture1);
    houseModel->materials[2].SetTexture(4, *plantsTexture1);
    houseModel->materials[3].SetTexture(4, *carTexture1);
    houseModel->materials[4].SetTexture(4, *plainTexture1);
    houseModel->materials[5].SetTexture(4, *plantsTexture1);

    houseModel->materials[0].SetTexture(7, *defaultNormalTexture);
    houseModel->materials[1].SetTexture(7, *defaultNormalTexture);
    houseModel->materials[2].SetTexture(7, *defaultNormalTexture);
    houseModel->materials[3].SetTexture(7, *defaultNormalTexture);
    houseModel->materials[4].SetTexture(7, *defaultNormalTexture);
    houseModel->materials[5].SetTexture(7, *defaultNormalTexture);

    houseModel->materials[0].SetProperty("material.shininess", 32.0f);
    houseModel->materials[1].SetProperty("material.shininess", 32.0f);
    houseModel->materials[2].SetProperty("material.shininess", 32.0f);
    houseModel->materials[3].SetProperty("material.shininess", 32.0f);
    houseModel->materials[4].SetProperty("material.shininess", 32.0f);
    houseModel->materials[5].SetProperty("material.shininess", 32.0f);

    material1->SetTexture(4, *diffuseTexture);
    material1->SetTexture(5, *specularTexture);
    material1->SetTexture(7, *normalTexture);
    material1->SetProperty("material.shininess", 32.0f);

    material2->SetTexture(4, *brickwallDiffuseTexture);
    material2->SetTexture(7, *brickwallNormalTexture);
    material2->SetTexture(8, *brickwallHeightTexture);
    material2->SetProperty("material.shininess", 32.0f);

    material3->SetTexture(4, *backpackDiffuseTexture);
    material3->SetTexture(5, *backpackSpecularTexture);
    material3->SetTexture(7, *backpackNormalTexture);
    material3->SetProperty("material.shininess", 32.0f);

    const auto lightFactory = new LightFactory();

    GameObject *ambientLight = lightFactory->SpawnAmbientLight();
    ambientLight->GetComponent<Light>()->color = Color(1.0f, 1.0f, 1.0f, 1.0f);
    ambientLight->GetComponent<Light>()->intensity = 0.06f;

    GameObject *sun = lightFactory->SpawnDirectionalLight();
    sun->GetComponent<Transform>()->position = Vector3(0, 40.0, 0);
    sun->GetComponent<Transform>()->rotation = Vector3(90, 0, 0);
    sun->GetComponent<Light>()->intensity = 2.0f;

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
    lamp2->GetComponent<Transform>()->position = Vector3(0, 6, -10);
    lamp2->GetComponent<Transform>()->rotation = Vector3(0, 0, 0);
    lamp2->GetComponent<PointLight>()->intensity = 4.0f;
    lamp2->GetComponent<PointLight>()->SetRange(10.0f);

    GameObject *lamp3 = lightFactory->SpawnPointLight();
    lamp3->GetComponent<Transform>()->position = Vector3(5, 7, 4);
    lamp3->GetComponent<Transform>()->rotation = Vector3(0, 0, 0);
    lamp3->GetComponent<PointLight>()->intensity = 4.0f;
    lamp3->GetComponent<PointLight>()->SetRange(10.0f);

    auto *scene = new Scene(camera);
    scene->Add(cube1);
    scene->Add(erika->root);
    scene->Add(houseModel->root);
    scene->Add(backpackModel->root);
    scene->Add(brickwall);
    
    scene->Add(ambientLight);
    scene->Add(sun);
    // scene->Add(light);
    // scene->Add(light2);
    // scene->Add(lamp);
    // // scene->Add(lamp2);
    // scene->Add(lamp3);

    scene->Init();

    double delta_time = 0;

    const auto controller = DIContainer::GetInstance().Get<InputController>();

    float t = 16;

    while (container->Get<IWindow>()->IsOpen()) {
        controller->Update();
        delta_time = container->Get<IWindow>()->GetDeltaTime();
        camera->Update(delta_time);
        ambientLight->GetComponent<Light>()->Update(delta_time);
        sun->GetComponent<Light>()->Update(delta_time);
        // sun->GetComponent<Transform>()->RotatePitch(delta_time * 30);
        t += delta_time * 127;
        //material1->SetProperty("material.shininess", 32);

        // brickwall->GetComponent<Transform>()->RotateYaw(10 * delta_time);
        cube1->GetComponent<Transform>()->RotateRoll(10 * delta_time);

        if (t >= 256) {
            t = 16;
        }

        renderer->BeginFrame(*camera);
        renderer->Render(*scene);
        renderer->EndFrame();
    }

    return 0;
}
