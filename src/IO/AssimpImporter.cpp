//
// Created by zykov on 4/26/2026.
//


#define GLM_ENABLE_EXPERIMENTAL

#include "AssimpImporter.h"
#include <iostream>
#include <ranges>

#include "../di/DIContainer.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/mesh.h"
#include "assimp/matrix4x4.h"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtc/quaternion.hpp"
#include "../components/MeshRenderer.h"

AssimpImporter::AssimpImporter()
    : importer(std::make_unique<Assimp::Importer>()),
      materialProperties{
          {ShaderProperty::COLOR_DIFFUSE,  {AI_MATKEY_COLOR_DIFFUSE,  ValueType::Vector4}},
          {ShaderProperty::COLOR_SPECULAR, {AI_MATKEY_COLOR_SPECULAR, ValueType::Vector4}},
          {ShaderProperty::SHININESS,      {AI_MATKEY_SHININESS,      ValueType::Float}},
      } {}

AssimpImporter::~AssimpImporter() = default;

Model *AssimpImporter::Import(const char *fileName) {
    const aiScene *scene = importer->ReadFile(
        fileName, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_OptimizeGraph );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer->GetErrorString() << std::endl;
        return {};
    }

    directory = std::string(fileName).substr(0, std::string(fileName).find_last_of('/'));

    auto *model = new Model;

    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        const aiMaterial *material = scene->mMaterials[i];
        ProcessMaterial(material, model);
        ProcessMaterialTexture(material);
    }

    auto *root = new GameObject(scene->mRootNode->mName.C_Str());
    auto *rootTransform = root->GetComponent<Transform>();
    rootTransform->position = Vector3::Zero();
    rootTransform->rotation = Vector3::Zero();
    rootTransform->scale = Vector3::One();

    model->root = root;

    ProcessNode(scene->mRootNode, scene, root, model);
    return model;
}

void AssimpImporter::ProcessNode(const aiNode *node, const aiScene *scene, GameObject* parent, Model* model) {
    const aiMatrix4x4 localTransform = node->mTransformation;

    aiVector3D aiScale, aiPosition;
    aiQuaternion aiRotation;
    localTransform.Decompose(aiScale, aiRotation, aiPosition);

    const glm::quat q(aiRotation.w, aiRotation.x, aiRotation.y, aiRotation.z);
    const glm::vec3 euler = glm::degrees(glm::eulerAngles(q));

    auto *main = new GameObject(node->mName.C_Str());
    auto *mainTransform = main->GetComponent<Transform>();
    mainTransform->position = Vector3(aiPosition.x, aiPosition.y, aiPosition.z);
    mainTransform->rotation = Vector3(euler.x, euler.y, euler.z);
    mainTransform->scale = Vector3(aiScale.x, aiScale.y, aiScale.z);
    auto *parentTransform = parent ? parent->GetTransform() : nullptr;
    mainTransform->SetParent(parentTransform);
    if (parentTransform) {
        parentTransform->AddChild(mainTransform);
    }

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        const aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];

        MeshEntry entry;
        const std::shared_ptr<Mesh> sharedMesh(ProcessMesh(mesh, scene));
        entry.mesh = sharedMesh;
        entry.position = Vector3(aiPosition.x, aiPosition.y, aiPosition.z);
        entry.rotation = Vector3(euler.x, euler.y, euler.z);
        entry.scale = Vector3(aiScale.x, aiScale.y, aiScale.z);
        entry.materialIndex = mesh->mMaterialIndex;
        model->meshes.push_back(entry);

        auto* child = new GameObject(mesh->mName.C_Str());
        auto* childTransformComponent = child->GetComponent<Transform>();
        childTransformComponent->position = Vector3::Zero();
        childTransformComponent->rotation = Vector3::Zero();
        childTransformComponent->scale = Vector3::One();
        mainTransform->AddChild(childTransformComponent);
        childTransformComponent->SetParent(mainTransform);
        auto* childMeshRenderer = child->AddComponent<MeshRenderer>();
        childMeshRenderer->mesh = sharedMesh;
        childMeshRenderer->material = &model->materials[mesh->mMaterialIndex];
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene, main, model);
    }
}

Mesh *AssimpImporter::ProcessMesh(const aiMesh *mesh, const aiScene *scene) {
    // data to fill
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        Vector3 vector;
        // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;
        // normals
        if (mesh->HasNormals()) {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
        }
        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            Vector2 vec{};
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = vec;
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.bitangent = vector;
        } else
            vertex.texCoords = Vector2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    // now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        const aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    const auto id = MeshId{Hash::Generate(mesh->mName.data)};

    return DIContainer::GetInstance().Get<Rendering::IRenderer>()->CreateMesh(vertices, indices);
}

void AssimpImporter::ProcessMaterial(const aiMaterial *mat, Model* model) {
    const auto standardShader = DIContainer::GetInstance().Get<Rendering::IRenderer>()->GetLitShader();

    Material material(standardShader, mat->GetName().C_Str());

    for (const auto key: standardShader->GetProperties() | std::views::keys) {
        auto [name, type, index, vType] = materialProperties[key];
        switch (vType) {
            case ValueType::Float: {
                float value;
                if (mat->Get(name, type, index, value) == AI_SUCCESS) {
                    material.SetProperty(key, value);
                }
                break;
            };
            case ValueType::Vector4: {
                aiColor4D value;
                if (mat->Get(name, type, index, value) == AI_SUCCESS) {
                    material.SetProperty(key, value);
                }
                break;
            }
        }
    }

    model->materials.push_back(material);

    // return;
    //
    // aiString materialName;
    // if (mat->Get(AI_MATKEY_NAME, materialName) == AI_SUCCESS) {
    //     std::cout << "Material Name: " << materialName.C_Str() << std::endl;
    // }
    //
    // aiColor3D diffuseColor(0.0f, 0.0f, 0.0f);
    // if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == AI_SUCCESS) {
    // }
    //
    // aiColor3D specularColor(0.0f, 0.0f, 0.0f);
    // mat->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
    //
    // float shininess = 0.0f;
    // mat->Get(AI_MATKEY_SHININESS, shininess);
    //
    // std::cout << "Material shininess: " << shininess << std::endl;
    //
    // int twoSided = 0;
    // mat->Get(AI_MATKEY_TWOSIDED, twoSided);
    //
    // std::cout << "Material twoSided: " << twoSided << std::endl;
    //
    // std::cout << "=== Total properties: " << mat->mNumProperties << " ===\n";
    //
    // for (unsigned int i = 0; i < mat->mNumProperties; ++i) {
    //     aiMaterialProperty *prop = mat->mProperties[i];
    //
    //     // ???? ???????? (?????????? ??? Assimp)
    //     std::string key = prop->mKey.C_Str();
    //     std::cout << "Key: " << key << " | ";
    //
    //     // ?????????? ??? ?????? ????????
    //     switch (prop->mType) {
    //         case aiPTI_Float: {
    //             // ???????? ????? ???? ???????? (????????, ???? ?? 3-4 float ??? ???????)
    //             unsigned int numElements = prop->mDataLength / sizeof(float);
    //             float *values = reinterpret_cast<float *>(prop->mData);
    //
    //             std::cout << "[Float] value(s): ";
    //             for (unsigned int j = 0; j < numElements; ++j) {
    //                 std::cout << values[j] << (j == numElements - 1 ? "" : ", ");
    //             }
    //             break;
    //         }
    //         case aiPTI_Double: {
    //             unsigned int numElements = prop->mDataLength / sizeof(double);
    //             double *values = reinterpret_cast<double *>(prop->mData);
    //
    //             std::cout << "[Double] value(s): ";
    //             for (unsigned int j = 0; j < numElements; ++j) {
    //                 std::cout << values[j] << (j == numElements - 1 ? "" : ", ");
    //             }
    //             break;
    //         }
    //         case aiPTI_String: {
    //             // ?????? ? Assimp ???????? ? ????????? ????? (?????? 4 ?????)
    //             // ?????????? ????? ???????????? ?????????? ????????? aiString
    //             aiString str;
    //             // ???????? ?????? ???????? ? aiString (????????? ??? ???????? ?????????)
    //             // ?? ? aiMaterial ???? ??????? ????? ??? ?????? ????? ?? ?????:
    //             if (mat->Get(key.c_str(), prop->mSemantic, prop->mIndex, str) == AI_SUCCESS) {
    //                 std::cout << "[String] value: " << str.C_Str();
    //             } else {
    //                 std::cout << "[String] (failed to parse)";
    //             }
    //             break;
    //         }
    //         case aiPTI_Integer: {
    //             unsigned int numElements = prop->mDataLength / sizeof(int);
    //             int *values = reinterpret_cast<int *>(prop->mData);
    //
    //             std::cout << "[Integer] value(s): ";
    //             for (unsigned int j = 0; j < numElements; ++j) {
    //                 std::cout << values[j] << (j == numElements - 1 ? "" : ", ");
    //             }
    //             break;
    //         }
    //         case aiPTI_Buffer: {
    //             std::cout << "[Binary Buffer] size: " << prop->mDataLength << " bytes";
    //             break;
    //         }
    //         default:
    //             std::cout << "[Unknown Type]";
    //             break;
    //     }
    //
    //     // ??????? ????????? (????? ??? ???????, ????????? ?? aiTextureType)
    //     // ? ?????? ???????? (????????, ?????? ????????)
    //     if (prop->mSemantic != 0) {
    //         std::cout << " | Semantic: " << prop->mSemantic << " | Index: " << prop->mIndex;
    //     }
    //
    //     std::cout << "\n";
    // }
    //
    // return;
}

Material *AssimpImporter::ProcessMaterialTexture(const aiMaterial *mat) {
    const unsigned int diffuseCount = mat->GetTextureCount(aiTextureType_DIFFUSE);

    for (unsigned int i = 0; i < diffuseCount; ++i) {
        aiString texturePath;

        if (mat->GetTexture(aiTextureType_DIFFUSE, i, &texturePath) == AI_SUCCESS) {
            std::cout << "Diffuse Texture Path: " << texturePath.C_Str() << std::endl;

            if (texturePath.data[0] == '*') {
                std::cout << "This is an embedded texture index!" << std::endl;
            }
        }
    }
    return nullptr;
}
