#include "cmake-build-debug/_deps/assimp-src/code/AssetLib/Blender/BlenderDNA.h"
#include "src/di/DIContainer.h"
#include "src/renderer/domain/Model.h"
#include "src/FPSCamera.h"
#include "src/Scene.h"
#include "src/Window.h"
#include "src/gameObject/Camera.h"
#include "src/gameObject/primitives/Cube.h"
#include "src/components/MeshRenderer.h"
#include "src/components/light/LightFactory.h"
#include "src/components/physics/collision/BoxCollider.h"
#include "src/components/light/PointLight.h"
#include "src/components/light/SpotLight.h"
#include "src/renderer/gl/GLCommandBuffer.h"

class MeshRenderer;
class Camera;
class BoxCollider;

int main() {
    constexpr int width = 1920;
    constexpr int height = 1080;

    DIContainer *container = &DIContainer::GetInstance();

    auto *camera = new FPSCamera(60, 0.01, 1000, static_cast<float>(width) / static_cast<float>(height));
    camera->GetTransform()->position = Vector3(10, 0, 10);

    auto *litShader = container->Get<AssetManager>()->Import<Shader>("phong/phong");
    litShader->AddProperty(ShaderProperty::SHININESS, ShaderMetaProperty(0, sizeof(float)));
    litShader->AddProperty(ShaderProperty::COLOR_DIFFUSE, ShaderMetaProperty(16, sizeof(Vector4)));

    const auto* diffuseTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(R"(assets/models/Survival_BackPack_2/1001_albedo.jpg)");
    const auto* specularTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(R"(assets/models/Survival_BackPack_2/1001_metallic.jpg)");

    Rendering::IGLTexture* diffuseTexture = new Rendering::GLTexture(diffuseTextureAsset->Width, diffuseTextureAsset->Height, diffuseTextureAsset->Wrap, diffuseTextureAsset->Data);
    Rendering::IGLTexture* specularTexture = new Rendering::GLTexture(specularTextureAsset->Width, specularTextureAsset->Height, diffuseTextureAsset->Wrap, specularTextureAsset->Data);

    //
    // stbi_image_free(diffuseTextureAsset->Data);
    // stbi_image_free(specularTextureAsset->Data);
    // delete diffuseTextureAsset;
    // delete specularTextureAsset;
    //

    auto *defaultMaterial = new Material(litShader);

    container->Get<Rendering::IRenderer>()->MaterialSetTexture(defaultMaterial->GetId(), "diffuse", diffuseTexture->GetId());
    container->Get<Rendering::IRenderer>()->MaterialSetTexture(defaultMaterial->GetId(), "specular", specularTexture->GetId());

    defaultMaterial->SetTexture("diffuse", *diffuseTexture);
    defaultMaterial->SetTexture("specular", *specularTexture);
    defaultMaterial->SetProperty("material.shininess", 32.0f);
    defaultMaterial->SetProperty("material.diffuseColor", Color(1.0f, 1.0f, 1.0f, 1.0f));

    // Building a glove
    const auto *gloveModel = container->Get<AssetManager>()->Import<Model>(
        R"(assets/models/Survival_BackPack_2/Survival_BackPack_2.fbx)");

    const auto glove = new GameObject();

    for (auto &entry: gloveModel->meshes) {
        const auto gloveTemp = new GameObject();
        auto *gloveMeshRenderer = gloveTemp->AddComponent<MeshRenderer>();
        gloveMeshRenderer->mesh = entry.mesh;
        gloveMeshRenderer->material = defaultMaterial;

        auto *gloveTransform = gloveTemp->GetTransform();

        gloveTransform->position = entry.position;
        gloveTransform->rotation = entry.rotation;
        gloveTransform->scale = entry.scale;

        gloveTransform->SetParent(glove->GetTransform());
        glove->GetTransform()->AddChild(gloveTransform);
    }

    glove->GetTransform()->position = Vector3(0, 0, 12);
    glove->GetTransform()->rotation = Vector3(0, 180, 0);
    glove->GetTransform()->scale = Vector3(0.005f, 0.005f, 0.005f);

    //character

    // const auto *characterModel = container->Get<AssetManager>()->Import<Model>(
    //   R"(assets/models/Erika.fbx)");
    //
    // const auto character = characterModel->root;

    //house

    auto *houseModel = container->Get<AssetManager>()->Import<Model>(
      R"(assets/models/autumn-house/source/House_scene_01.fbx)");

    const auto house = houseModel->root;

    diffuseTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(R"(assets/models/autumn-house/textures/House_texture_01_00.png)");
    specularTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(R"(assets/models/autumn-house/textures/House_texture_02_00.png)");

    diffuseTexture = new Rendering::GLTexture(diffuseTextureAsset->Width, diffuseTextureAsset->Height, diffuseTextureAsset->Wrap, diffuseTextureAsset->Data);
    specularTexture = new Rendering::GLTexture(specularTextureAsset->Width, specularTextureAsset->Height, specularTextureAsset->Wrap, specularTextureAsset->Data);

    for (auto& material : houseModel->materials) {
        material.SetTexture("diffuse", *diffuseTexture);
        material.SetTexture("specular", *specularTexture);
        material.SetProperty("material.shininess", 32.0f);
        material.SetProperty("material.diffuseColor", Color(1.0f, 1.0f, 1.0f, 1.0f));
        container->Get<Rendering::IRenderer>()->MaterialSetTexture(material.GetId(), "diffuse", diffuseTexture->GetId());
        container->Get<Rendering::IRenderer>()->MaterialSetTexture(material.GetId(), "specular", specularTexture->GetId());
    }

    defaultMaterial = new Material(litShader);

    defaultMaterial->SetTexture("diffuse", *diffuseTexture);
    defaultMaterial->SetTexture("specular", *specularTexture);
    defaultMaterial->SetProperty("material.shininess", 32.0f);
    defaultMaterial->SetProperty("material.diffuseColor", Color(1.0f, 1.0f, 1.0f, 1.0f));

    house->GetTransform()->position = Vector3(0, -2, 80);
    house->GetTransform()->rotation = Vector3(0, 180, 0);
    house->GetTransform()->scale = Vector3(1.0f, 1.0f, 1.0f);

    // end house

    //
    // stbi_image_free(diffuseTextureAsset->Data);
    // stbi_image_free(specularTextureAsset->Data);
    // delete diffuseTextureAsset;
    // delete specularTextureAsset;
    //

    diffuseTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(R"(assets/models/glove/texture/wood_with_steel_diffuse.png)");
    specularTextureAsset = container->Get<AssetManager>()->Import<TextureAsset>(R"(assets/models/glove/texture/wood_with_steel_specular.png)");

    diffuseTexture = new Rendering::GLTexture(diffuseTextureAsset->Width, diffuseTextureAsset->Height, diffuseTextureAsset->Wrap, diffuseTextureAsset->Data);
    specularTexture = new Rendering::GLTexture(specularTextureAsset->Width, specularTextureAsset->Height, diffuseTextureAsset->Wrap, specularTextureAsset->Data);

    defaultMaterial = new Material(litShader);

    container->Get<Rendering::IRenderer>()->MaterialSetTexture(defaultMaterial->GetId(), "diffuse", diffuseTexture->GetId());
    container->Get<Rendering::IRenderer>()->MaterialSetTexture(defaultMaterial->GetId(), "specular", specularTexture->GetId());

    defaultMaterial->SetTexture("diffuse", *diffuseTexture);
    defaultMaterial->SetTexture("specular", *specularTexture);
    defaultMaterial->SetProperty("material.shininess", 512.0f);
    defaultMaterial->SetProperty("material.diffuseColor", Color(1.0f, 1.0f, 1.0f, 1.0f));

    const auto cube1 = Cube::BuildCube();
    cube1->GetComponent<MeshRenderer>()->material = defaultMaterial;
    cube1->AddComponent<BoxCollider>();
    cube1->GetTransform()->position = Vector3(10, 0, 0);

    const auto cube2 = Cube::BuildCube();
    //cube2->AddComponent<BoxCollider>();
    cube2->GetComponent<MeshRenderer>()->material = defaultMaterial;
    cube2->GetTransform()->position = Vector3(10, 0, -10);

    const auto cube3 = Cube::BuildCube();
    cube3->GetComponent<MeshRenderer>()->material = defaultMaterial;
    cube3->GetTransform()->position = Vector3(10, 0, 4);

    const auto cube4 = Cube::BuildCube();
    cube4->GetComponent<MeshRenderer>()->material = defaultMaterial;
    cube4->GetTransform()->position = Vector3(10, 0, 8);

    const auto cube5 = Cube::BuildCube();
    cube5->GetComponent<MeshRenderer>()->material = defaultMaterial;
    cube5->GetTransform()->position = Vector3(0, -3, 0);
    cube5->GetTransform()->scale = Vector3(100, 0.1f, 100);

    const auto lightFactory = new LightFactory();
    GameObject *ambientLight = lightFactory->SpawnAmbientLight();
    ambientLight->GetComponent<Light>()->intensity = 0.25f;
    GameObject *sun = lightFactory->SpawnDirectionalLight();
    sun->GetComponent<Transform>()->position = Vector3(10, 20, 0);
    sun->GetComponent<Transform>()->rotation = Vector3(90, 0, 0);
    sun->GetComponent<Light>()->intensity = 2.0f;
    GameObject *pointLight = lightFactory->SpawnPointLight();
    pointLight->GetComponent<Transform>()->position = Vector3(0, 0.0f, 2);
    pointLight->GetComponent<PointLight>()->intensity = 1.0f;
    pointLight->GetComponent<PointLight>()->SetRange(3.0f);
    GameObject *flashLight = lightFactory->SpawnSpotLight();
    flashLight->GetComponent<Transform>()->position = Vector3(0, 2.0f, 0);
    flashLight->GetComponent<Transform>()->rotation = Vector3(90, 0, 0);
    flashLight->GetComponent<SpotLight>()->intensity = 1.0f;

    auto *scene = new Scene(camera);
    scene->Add(ambientLight);
    scene->Add(sun);
    scene->Add(flashLight);
    scene->Add(pointLight);
    scene->Add(cube1);
    scene->Add(cube2);
    scene->Add(cube3);
    scene->Add(cube4);
    scene->Add(cube5);
    scene->Add(glove);
    scene->Add(house);
    // scene->Add(character);

    container->Get<IWindow>()->AddView(*scene);
    container->Get<IWindow>()->Update();

    return 0;
}
