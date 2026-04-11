#include <iostream>

#include "src/DIContainer.h"
#include "src/FPSCamera.h"
#include "src/Scene.h"
#include "src/Window.h"
#include "src/gameObject/Camera.h"
#include "src/gameObject/primitives/Cube.h"
#include "src/graphics/DefaultMaterial.h"
#include "src/components/MeshRenderer.h"
#include "src/math/Matrix3x3.h"

class MeshRenderer;
class Camera;

int main() {
    Matrix3x3 mat = Matrix3x3();
    mat.m[0][0] = 1.0f;     mat.m[1][0] = 2.0f;     mat.m[2][0] = 3.0f;
    mat.m[0][1] = 0.0f;     mat.m[1][1] = 4.0f;     mat.m[2][1] = 5.0f;
    mat.m[0][2] = 1.0f;     mat.m[1][2] = 0.0f;     mat.m[2][2] = 6.0f;

    // mat.m[0][0] = 5.0f;     mat.m[1][0] = 2.0f;     mat.m[2][0] = 0.0f;
    // mat.m[0][1] = 3.0f;     mat.m[1][1] = 0.0f;     mat.m[2][1] = 1.0f;
    // mat.m[0][2] = 1.0f;     mat.m[1][2] = 2.0f;     mat.m[2][2] = 2.0f;

    float det = mat.Determinant();

    std::cout << "det: " << det << std::endl;

    //return 0;

    Matrix4x4 mat2 = Matrix4x4(0);
    mat2.m[0][0] = 2.0f;     mat2.m[1][0] = 0.0f;     mat2.m[2][0] = 3.0f;     mat2.m[3][0] = 1.0f;
    mat2.m[0][1] = 1.0f;     mat2.m[1][1] = 5.0f;     mat2.m[2][1] = 2.0f;     mat2.m[3][1] = 0.0f;
    mat2.m[0][2] = 0.0f;     mat2.m[1][2] = 3.0f;     mat2.m[2][2] = 0.0f;     mat2.m[3][2] = 1.0f;
    mat2.m[0][3] = 4.0f;     mat2.m[1][3] = 1.0f;     mat2.m[2][3] = 2.0f;     mat2.m[3][3] = 2.0f;

    float det2 = mat2.Determinant();

    std::cout << "det2: " << det2 << std::endl;

    float det3  = mat2.Minor(3, 3).Determinant();

    std::cout << "det3: " << det3 << std::endl;

    Matrix4x4 mat3 = mat2.Inverse();

    Matrix4x4 mat4 = Matrix4x4(0);
    mat4 = mat3 * mat2;

    // return 0;

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
    const auto cube3 = Cube::BuildCube();
    cube3->GetComponent<MeshRenderer>()->material = defaultMaterial;

    auto* scene = new Scene(camera);
    scene->AddLight(light);
    scene->AddGameObject(cube1);
    scene->AddGameObject(cube2);
    scene->AddGameObject(cube3);
    scene->AddGameObject(glove);

    cube1->GetComponent<Transform>()->Translate(cube1->GetComponent<Transform>()->Forward() * -1);
    cube2->GetComponent<Transform>()->Translate(cube2->GetComponent<Transform>()->Forward() * 4);
    cube3->GetComponent<Transform>()->Translate(cube3->GetComponent<Transform>()->Forward() * 6);

    cube2->GetComponent<Transform>()->SetParent(cube1->GetComponent<Transform>());
    cube3->GetComponent<Transform>()->SetParent(cube2->GetComponent<Transform>());
    // cube2->GetComponent<Transform>()->SetParent(nullptr);
    // cube3->GetComponent<Transform>()->SetParent(nullptr);

    window->AttachScene(scene);

    // scene->Init();

    while (!window->IsClosed()) {
        window->Update();
    }

    return 0;
}
