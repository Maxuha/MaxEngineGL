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
#include "src/gameObject/primitives/Sphere.h"
#include "src/IO/FileReader.h"
#include "src/renderer/domain/Cubemap.h"
#include "src/renderer/DrawCall.h"
#include <iostream>

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
    auto skyboxShader = assetManager->Import<Shader>("skybox/skybox", {{"renderFace", "Both"}});
    auto reflactionShader = assetManager->Import<Shader>("reflaction/reflaction", {{"renderFace", "Both"}});
    auto refractionShader = assetManager->Import<Shader>("refraction/refraction", {{"renderFace", "Both"}});
    auto pbrShader = assetManager->Import<Shader>("pbr/pbr", {{"renderFace", "Both"}});
    auto envShader = assetManager->Import<Shader>("env/env", {{"renderFace", "Both"}});

    renderer->SetDepthShader(*depthShader);

    auto material1 = new Material(phongShader, 32);
    auto material2 = new Material(phongShader, 32);
    auto material3 = new Material(phongShader, 32);
    auto pbrMaterial = new Material(pbrShader, 512);
    auto rustedironMaterial = new Material(pbrShader, 512);
    auto skyboxMaterial = new Material(skyboxShader, 0);
    auto reflactionMaterial = new Material(reflactionShader, 0);
    auto refractionMaterial = new Material(refractionShader, 0);
    auto envMaterial = new Material(envShader, 512, "Env Material");

    renderer->SetSkyboxMaterial(*skyboxMaterial);

    auto *camera = new FPSCamera(60, 0.01, 1000, static_cast<float>(width) / static_cast<float>(height));
    camera->GetTransform()->position = Vector3(0, 0, 0);
    camera->GetTransform()->rotation = Quaternion::Identity();
    camera->Start();

    const auto cube1 = Cube::BuildCube();
    cube1->GetComponent<MeshRenderer>()->material = material1;
    cube1->GetComponent<Transform>()->position = Vector3(-1.0f, 4.0f, 8.0f);

    const auto cube2 = Cube::BuildCube();
    cube2->GetComponent<MeshRenderer>()->material = reflactionMaterial;
    cube2->GetComponent<Transform>()->position = Vector3(3, 4.0f, 8.0f);

    const auto cube3 = Cube::BuildCube();
    cube3->GetComponent<MeshRenderer>()->material = refractionMaterial;
    cube3->GetComponent<Transform>()->position = Vector3(7, 4.0f, 8.0f);

    const auto envCube = Cube::BuildCube();
    envCube->GetComponent<MeshRenderer>()->material = envMaterial;
    envCube->GetComponent<Transform>()->position = Vector3(0.0f, 0.0f, 0.0f);

    const auto sphere = Sphere::BuildSphere();
    sphere->GetComponent<MeshRenderer>()->material = rustedironMaterial;
    sphere->GetComponent<Transform>()->position = Vector3(8, 4.0f, 5.0f);

    const auto brickwall = Plane::BuildPlane();
    brickwall->GetComponent<MeshRenderer>()->material = material2;
    brickwall->GetComponent<Transform>()->position = Vector3(20, 7.0f, 0.0f);
    brickwall->GetComponent<Transform>()->rotation = Quaternion::FromEulerAngles(90.0f, 0.0f, 0.0f);
    brickwall->GetComponent<Transform>()->scale = Vector3(5.0f, 5.0f, 1.0f);

    auto *houseModel = container->Get<AssetManager>()->Import<Model>(
        R"(assets/models/autumn-house/source/House_scene_01.fbx)");
    houseModel->root->GetTransform()->position = Vector3(0, 0, 0);
    houseModel->root->GetTransform()->rotation = Quaternion::Identity();

    auto *backpackModel = container->Get<AssetManager>()->Import<Model>(
        R"(assets/models/Survival_BackPack_2/Survival_BackPack_2.fbx)");

    uint32_t size = backpackModel->root->GetComponentsInChildren<MeshRenderer>().size();

    for (int i = 0; i < size; i++) {
        backpackModel->root->GetComponentsInChildren<MeshRenderer>()[i]->material = pbrMaterial;
    }

    backpackModel->root->GetTransform()->position = Vector3(5, 5, -8);
    backpackModel->root->GetTransform()->rotation = Quaternion::Identity();
    backpackModel->root->GetTransform()->scale = Vector3(0.01, 0.01, 0.01);

    auto *backpackModel2 = container->Get<AssetManager>()->Import<Model>(
       R"(assets/models/Survival_BackPack_2/Survival_BackPack_2.fbx)");

    for (int i = 0; i < size; i++) {
        backpackModel2->root->GetComponentsInChildren<MeshRenderer>()[i]->material = material3;
    }

    backpackModel2->root->GetTransform()->position = Vector3(10, 5, -8);
    backpackModel2->root->GetTransform()->rotation = Quaternion::Identity();
    backpackModel2->root->GetTransform()->scale = Vector3(0.01, 0.01, 0.01);

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

    TextureAsset *rustedironDiffuseTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(
      R"(assets/models/rustediron/rustediron2_basecolor.png)");

    Rendering::Texture *rustedironDiffuseTexture = new Rendering::Texture(rustedironDiffuseTextureAsset->Width, rustedironDiffuseTextureAsset->Height,
                                                                rustedironDiffuseTextureAsset->Wrap, rustedironDiffuseTextureAsset->Data);

    TextureAsset *rustedironMetallicTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(
  R"(assets/models/rustediron/rustediron2_metallic.png)");

    Rendering::Texture *rustedironMetallicTexture = new Rendering::Texture(rustedironMetallicTextureAsset->Width, rustedironMetallicTextureAsset->Height,
                                                                rustedironMetallicTextureAsset->Wrap, rustedironMetallicTextureAsset->Data);

    TextureAsset *rustedironNormalTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(
  R"(assets/models/rustediron/rustediron2_normal.png)");

    Rendering::Texture *rustedironNormalTexture = new Rendering::Texture(rustedironNormalTextureAsset->Width, rustedironNormalTextureAsset->Height,
                                                                rustedironNormalTextureAsset->Wrap, rustedironNormalTextureAsset->Data);

    TextureAsset *rustedironRoughnessTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(
R"(assets/models/rustediron/rustediron2_roughness.png)");

    Rendering::Texture *rustedironRoughnessTexture = new Rendering::Texture(rustedironRoughnessTextureAsset->Width, rustedironRoughnessTextureAsset->Height,
                                                                rustedironRoughnessTextureAsset->Wrap, rustedironRoughnessTextureAsset->Data);

    TextureAsset *backpackDiffuseTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(
      R"(assets/models/Survival_BackPack_2/1001_albedo.jpg)");

    Rendering::Texture *backpackDiffuseTexture = new Rendering::Texture(backpackDiffuseTextureAsset->Width, backpackDiffuseTextureAsset->Height,
                                                                backpackDiffuseTextureAsset->Wrap, backpackDiffuseTextureAsset->Data);

    TextureAsset *backpackSpecularTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(
   R"(assets/models/Survival_BackPack_2/1001_metallic.jpg)");

    Rendering::Texture *backpackSpecularTexture = new Rendering::Texture(backpackSpecularTextureAsset->Width, backpackSpecularTextureAsset->Height,
                                                                backpackSpecularTextureAsset->Wrap, backpackSpecularTextureAsset->Data, Rendering::TextureFormat::RGBA);

    TextureAsset *backpackRougnessTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(
   R"(assets/models/Survival_BackPack_2/1001_roughness.jpg)");

    Rendering::Texture *backpackRougnessTexture = new Rendering::Texture(backpackRougnessTextureAsset->Width, backpackRougnessTextureAsset->Height,
                                                                backpackRougnessTextureAsset->Wrap, backpackRougnessTextureAsset->Data, Rendering::TextureFormat::RGBA);

    TextureAsset *backpackAOTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(
   R"(assets/models/Survival_BackPack_2/1001_AO.jpg)");

    Rendering::Texture *backpackAOTexture = new Rendering::Texture(backpackAOTextureAsset->Width, backpackAOTextureAsset->Height,
                                                                backpackAOTextureAsset->Wrap, backpackAOTextureAsset->Data, Rendering::TextureFormat::RGBA);

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

    //skybox
    TextureAsset *skyboxRightTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(
        R"(assets/textures/skybox/right.jpg)");


    TextureAsset *skyboxLeftTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(
       R"(assets/textures/skybox/left.jpg)");

    TextureAsset *skyboxTopTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(
      R"(assets/textures/skybox/top.jpg)");


    TextureAsset *skyboxBottomTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(
     R"(assets/textures/skybox/bottom.jpg)");


    TextureAsset *skyboxFrontTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(
    R"(assets/textures/skybox/front.jpg)");

    TextureAsset *skyboxBackTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(
  R"(assets/textures/skybox/back.jpg)");

    std::vector<void*> skyboxTextures = { skyboxRightTextureAsset->Data, skyboxLeftTextureAsset->Data, skyboxTopTextureAsset->Data, skyboxBottomTextureAsset->Data, skyboxFrontTextureAsset->Data, skyboxBackTextureAsset->Data };

    Rendering::Cubemap *skybox = new Rendering::Cubemap(skyboxBackTextureAsset->Width, Rendering::TextureFormat::SRGBA, skyboxTextures);

    skyboxMaterial->SetCubemap(3, *skybox);
    reflactionMaterial->SetCubemap(3, *skybox);
    refractionMaterial->SetCubemap(3, *skybox);
    envMaterial->SetCubemap(1, *skybox);

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

    pbrMaterial->SetTexture(4, *backpackDiffuseTexture);
    pbrMaterial->SetTexture(5, *backpackSpecularTexture);
    pbrMaterial->SetTexture(7, *backpackNormalTexture);
    // pbrMaterial->SetTexture(8, *brickwallHeightTexture);
    pbrMaterial->SetTexture(9, *backpackDiffuseTexture);
    pbrMaterial->SetTexture(10, *backpackSpecularTexture);
    pbrMaterial->SetTexture(11, *backpackRougnessTexture);
    pbrMaterial->SetTexture(12, *backpackAOTexture);
    pbrMaterial->SetCubemap(13, *skybox);

    pbrMaterial->SetProperty("material.shininess", 32.0f);
    pbrMaterial->SetProperty("material.albedo", Vector3(1.0f, 1.0f, 1.0f));
    pbrMaterial->SetProperty("material.metallic", 1.0f);
    pbrMaterial->SetProperty("material.roughness", 0.3f);
    pbrMaterial->SetProperty("material.ao", 1.0f);

    rustedironMaterial->SetTexture(4, *rustedironDiffuseTexture);
    rustedironMaterial->SetTexture(5, *rustedironMetallicTexture);
    rustedironMaterial->SetTexture(7, *rustedironNormalTexture);
    // rustedironMaterial->SetTexture(8, *brickwallHeightTexture);
    rustedironMaterial->SetTexture(9, *rustedironDiffuseTexture);
    rustedironMaterial->SetTexture(10, *rustedironMetallicTexture);
    rustedironMaterial->SetTexture(11, *rustedironRoughnessTexture);
    rustedironMaterial->SetTexture(12, *backpackAOTexture);
    rustedironMaterial->SetCubemap(13, *skybox);

    rustedironMaterial->SetProperty("material.shininess", 32.0f);
    rustedironMaterial->SetProperty("material.albedo", Vector3(1.0f, 1.0f, 1.0f));
    rustedironMaterial->SetProperty("material.metallic", 0.5f);
    rustedironMaterial->SetProperty("material.roughness", 0.3f);
    rustedironMaterial->SetProperty("material.ao", 1.0f);

    const auto lightFactory = new LightFactory();

    GameObject *ambientLight = lightFactory->SpawnAmbientLight();
    ambientLight->GetComponent<Light>()->color = Color(1.0f, 1.0f, 1.0f, 1.0f);
    ambientLight->GetComponent<Light>()->intensity = 0.06f;

    GameObject *sun = lightFactory->SpawnDirectionalLight();
    sun->GetComponent<Transform>()->position = Vector3(0, 40.0, 0);
    sun->GetComponent<Transform>()->rotation = Quaternion::FromEulerAngles(90, 0, 0);
    sun->GetComponent<Light>()->intensity = 2.5f;

    GameObject *light = lightFactory->SpawnSpotLight();
    light->GetComponent<Transform>()->position = Vector3(0, 6, -10);
    light->GetComponent<Transform>()->rotation = Quaternion::FromEulerAngles(90, 0, 0);
    light->GetComponent<SpotLight>()->intensity = 4.0f;
    light->GetComponent<SpotLight>()->SetInnerAngle(3.0f);
    light->GetComponent<SpotLight>()->SetOuterAngle(10.0f);
    light->GetComponent<SpotLight>()->SetRange(10.0f);

    GameObject *light2 = lightFactory->SpawnSpotLight();
    light2->GetComponent<Transform>()->position = Vector3(-1, 6, 10);
    light2->GetComponent<Transform>()->rotation = Quaternion::FromEulerAngles(90, 0, 0);
    light2->GetComponent<SpotLight>()->intensity = 4.0f;
    light2->GetComponent<SpotLight>()->SetInnerAngle(3.0f);
    light2->GetComponent<SpotLight>()->SetOuterAngle(10.0f);
    light2->GetComponent<SpotLight>()->SetRange(10.0f);

    GameObject *lamp = lightFactory->SpawnPointLight();
    lamp->GetComponent<Transform>()->position = Vector3(0, 2, 0);
    lamp->GetComponent<Transform>()->rotation = Quaternion::FromEulerAngles(0, 0, 0);
    lamp->GetComponent<PointLight>()->intensity = 4.0f;
    lamp->GetComponent<PointLight>()->SetRange(10.0f);

    GameObject *lamp2 = lightFactory->SpawnPointLight();
    lamp2->GetComponent<Transform>()->position = Vector3(0, 6, -10);
    lamp2->GetComponent<Transform>()->rotation = Quaternion::FromEulerAngles(0, 0, 0);
    lamp2->GetComponent<PointLight>()->intensity = 4.0f;
    lamp2->GetComponent<PointLight>()->SetRange(10.0f);

    GameObject *lamp3 = lightFactory->SpawnPointLight();
    lamp3->GetComponent<Transform>()->position = Vector3(5, 7, 4);
    lamp3->GetComponent<Transform>()->rotation = Quaternion::FromEulerAngles(0, 0, 0);
    lamp3->GetComponent<PointLight>()->intensity = 4.0f;
    lamp3->GetComponent<PointLight>()->SetRange(10.0f);

    Rendering::DrawCall draw_call = Rendering::DrawCall { ._mesh = envCube->GetComponent<MeshRenderer>()->mesh.get(), .material = envCube->GetComponent<MeshRenderer>()->material , .transform = envCube->GetTransform()};

     renderer->Bake(draw_call);

    auto *scene = new Scene(camera);
    //  scene->Add(envCube);
    scene->Add(cube1);
     scene->Add(cube2);
     scene->Add(cube3);
      scene->Add(sphere);
     // scene->Add(erika->root);
     scene->Add(houseModel->root);
     scene->Add(backpackModel->root);
    scene->Add(backpackModel2->root);
    scene->Add(brickwall);
    
    scene->Add(ambientLight);
    scene->Add(sun);
    // scene->Add(light);
    // scene->Add(light2);
    // scene->Add(lamp);
    // scene->Add(lamp2);
    // scene->Add(lamp3);

    scene->Init();

    double delta_time = 0;

    const auto controller = DIContainer::GetInstance().Get<InputController>();

    float t = 16;

    float angle = 0;

    // backpackModel->root->GetComponent<Transform>()->rotation = Quaternion::FromEulerAngles(180, 0, 0);

    while (container->Get<IWindow>()->IsOpen()) {
        controller->Update();
        delta_time = container->Get<IWindow>()->GetDeltaTime();
        camera->Update(delta_time);
        ambientLight->GetComponent<Light>()->Update(delta_time);

         // sun->GetComponent<Transform>()->RotatePitch(delta_time * 30);

        backpackModel->root->GetComponent<Transform>()->rotation = backpackModel->root->GetComponent<Transform>()->rotation * Quaternion::FromAxisAngle(Vector3::Up().x, Vector3::Up().y, Vector3::Up().z, 10 * delta_time);
        cube1->GetComponent<Transform>()->rotation = Quaternion::FromAxisAngle(cube1->GetComponent<Transform>()->Right().x, cube1->GetComponent<Transform>()->Right().y, cube1->GetComponent<Transform>()->Right().z, angle);
        sphere->GetComponent<Transform>()->rotation = Quaternion::FromAxisAngle(sphere->GetComponent<Transform>()->Up().x, sphere->GetComponent<Transform>()->Up().y, sphere->GetComponent<Transform>()->Up().z, angle);
        sphere->GetComponent<Transform>()->rotation = Quaternion::FromEulerAngles(0, angle, 0);
        angle += 30 * delta_time;

        renderer->BeginFrame(*camera);
        renderer->Render(*scene);
        renderer->EndFrame();

        std::cout << "Position: " << camera->GetComponent<Transform>()->position << std::endl;
    }

    return 0;
}
